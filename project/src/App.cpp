#include "App.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>
#include <stdexcept>
#include <array>
#include <chrono>
#include <memory>

#include <engine/scene/components/Mesh.h>
#include <engine/scene/components/RigidBody2D.h>
#include <engine/input/Input.h>
#include <engine/renderer/MemBuffer.h>
#include <engine/renderer/FrameInfo.h>
#include <engine/renderer/systems/Simple.h>
#include <engine/renderer/systems/Billboards.h>
#include <engine/renderer/systems/Lighting.h>
#include <systems/Profiler.hpp>

using namespace Scop;

App* App::instance = nullptr;

App::App() {
  this->globalDescriptorPool = Renderer::DescriptorPool::Builder(this->device)
    .setMaxSets(Renderer::Swapchain::MAX_FRAMES_IN_FLIGHT)
    .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Renderer::Swapchain::MAX_FRAMES_IN_FLIGHT)
    .build();
  App::instance = this;
  Input::Init(this->window.getHandle());
}
App::~App() {}

void App::run() {
  std::vector<std::unique_ptr<Renderer::MemBuffer>> globalUboBuffers(Renderer::Swapchain::MAX_FRAMES_IN_FLIGHT);
  for (auto& buffer : globalUboBuffers) {
    buffer = std::make_unique<Renderer::MemBuffer>(
      this->device,
      sizeof(Renderer::GlobalUbo),
      1,
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
    );
    buffer->map();
  }

  auto globalSetLayout = Renderer::DescriptorSetLayout::Builder(this->device)
    .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
    .build();

  std::vector<VkDescriptorSet> globalDescriptorSets(Renderer::Swapchain::MAX_FRAMES_IN_FLIGHT);
  for (uint32_t i = 0; i < Renderer::Swapchain::MAX_FRAMES_IN_FLIGHT; ++i) {
    auto bufferInfo = globalUboBuffers[i]->getDescriptorInfo();
    Renderer::DescriptorWriter(*globalSetLayout, *globalDescriptorPool)
      .write(0, &bufferInfo)
      .build(globalDescriptorSets[i]);
  }
  Renderer::Systems::SystemInfo systemInfo{
    this->device,
    this->renderer.getSwapchainRenderPass(),
    globalSetLayout->getHandle()
  };
  Renderer::Systems::Simple simpleRenderSystem(systemInfo);
  Renderer::Systems::Billboards billboardsSystem(systemInfo);
  Renderer::Systems::Lighting lightingSystem;
  Profiler profiler;

  this->sceneCamera.setPerspective(glm::radians(50.f), .1f, 100.f);
  this->sceneCamera.setViewYXZ(glm::vec3{ .88f, -0.95f, -1.95f }, glm::vec3{ 0.41f, 3.17f, 0.f });

  auto currentTime = std::chrono::high_resolution_clock::now();
  Input::SetMouseMode(Input::MouseMode::Hidden);
  while (!this->window.shouldClose()) {
    Input::Update();
    this->window.pollEvents();
    this->sceneCamera.setAspectRatio(this->renderer.getSwapchainExtentAspectRatio());
    auto newTime = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
    currentTime = newTime;
    this->sceneCamera.update(deltaTime);
    auto cmdBuffer = this->renderer.beginFrame();
    if (!cmdBuffer)
      continue;
    auto frameIndex = this->renderer.getFrameIndex();
    Renderer::FrameInfo frameInfo{
      deltaTime,
      frameIndex,
      cmdBuffer,
      this->sceneCamera,
      globalDescriptorSets[frameIndex],
      {}
    };

    // update
    Renderer::GlobalUbo& ubo = frameInfo.globalUbo;
    ubo.projection = this->sceneCamera.getProjection();
    ubo.view = this->sceneCamera.getView();
    ubo.projectionView = this->sceneCamera.getProjectionView();
    ubo.inverseView = this->sceneCamera.getInverseView();
    lightingSystem.update(frameInfo, this->scene);
    profiler.update(frameInfo.deltaTime);

    globalUboBuffers[frameIndex]->writeTo(&ubo);
    globalUboBuffers[frameIndex]->flush();

    // render
    this->renderer.beginSwapchainRenderPass(cmdBuffer);
    simpleRenderSystem.render(frameInfo, this->scene);
    billboardsSystem.render(frameInfo, this->scene);
    this->renderer.endSwapchainRenderPass(cmdBuffer);
    this->renderer.endFrame();
    // vkDeviceWaitIdle(this->device.getHandle());
  }
  vkDeviceWaitIdle(this->device.getHandle());
}

template <typename T>
static Entity CreateLight(
  Scene& scene,
  const std::string_view tag,
  glm::vec3 position,
  glm::vec4 color,
  bool addBillboard = false,
  float range = 10.0f
) {
  Entity entity = scene.createEntity(tag);
  entity.transform().translation = position;
  entity.addComponent<T>(color, range);
  if (addBillboard)
    entity.addComponent<Components::Billboard>(color, glm::vec2{ .02f }, true);
  return entity;
}

#include <glm/gtc/color_space.hpp>
#include <utils/rgb.h>
static std::vector<glm::vec3> GenerateRainbowColors(int count) {
  std::vector<glm::vec3> colors;
  for (int i = 0; i < count; i++) {
    float hue = i / static_cast<float>(count) * 360.f;
    glm::vec3 hsvColor = glm::vec3(hue, 1.f, 1.0f); // HSV color
    glm::vec3 rgbColor = Utils::Hsv2rgb(hsvColor);
    colors.push_back(rgbColor);
  }
  return colors;
}

void App::loadEntities(const std::vector<std::string_view>& modelPaths) {
  uint32_t i = 0;
  for (const auto path : modelPaths) {
    std::shared_ptr<Renderer::Model> cubeModel = Renderer::Model::CreateFromFile(this->device, path);
    auto entity = this->scene.createEntity("Cube");
    entity.addComponent<Components::Mesh>(cubeModel);
    auto& transform = entity.transform();
    transform.translation = { -0.5f + 1.f * i++, 0.5f, 0.f };
    // transform.rotation = { 0.f, 0.f, glm::pi<float>() };
    transform.scale = glm::vec3{ 3.f, 1.5f, 3.f };
  }

  auto floor = this->scene.createEntity("Floor");
  auto floorModel = Renderer::Model::CreateFromFile(this->device, "assets/models/quad.obj");
  floor.addComponent<Components::Mesh>(floorModel);
  auto& floorTransform = floor.transform();
  floorTransform.translation = { 0.f, .5f, 0.f };
  floorTransform.scale = { 12.f, 1.f, 12.f };

  CreateLight<Components::GlobalLight>(
    this->scene, "Ambient Light",
    glm::vec3{ .0f },
    glm::vec4{ 1.f,1.f, 1.f, .02f }
  );
  // CreateLight<Components::PointLight>(
  //   this->scene, "Point Light",
  //   glm::vec3{ 1.f, 0.f, 0.f },
  //   glm::vec4{ .9f,.5f, .3f, .25f },
  //   true,
  //   10.f
  // );
  std::vector<glm::vec3> lightColors = GenerateRainbowColors(16);

  for (uint32_t i = 0; i < lightColors.size(); i++) {
    auto rotation = glm::rotate(
      glm::mat4(1.f),
      (i * glm::two_pi<float>()) / lightColors.size(),
      { 0.f, -1.f, 0.f });
    auto newPosition = glm::vec3(rotation * glm::vec4(-1.f, -1.f, -1.f, 1.f));
    CreateLight<Components::PointLight>(
      this->scene, "Point Light",
      newPosition,
      glm::vec4{ lightColors[i], .25f },
      true,
      100.f
    );
  }
  // CreateLight<Components::PointLight>

}

