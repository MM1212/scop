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

using namespace Scop;

struct GlobalUbo {
  alignas(16) glm::mat4 projectionView{ 1.f };
  alignas(16) glm::vec3 globalLightDirection = glm::normalize(glm::vec3{ 1.f, -3.f, -1.f });
  alignas(16) glm::vec3 globalLightColor = glm::vec3{1.f, .7f, .5f };
};

App* App::instance = nullptr;

App::App() {
  this->globalDescriptorPool = Renderer::DescriptorPool::Builder(this->device)
    .setMaxSets(Renderer::Swapchain::MAX_FRAMES_IN_FLIGHT)
    .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Renderer::Swapchain::MAX_FRAMES_IN_FLIGHT)
    .build();
  App::instance = this;
}
App::~App() {}

void App::run() {
  std::vector<std::unique_ptr<Renderer::MemBuffer>> globalUboBuffers(Renderer::Swapchain::MAX_FRAMES_IN_FLIGHT);
  for (auto& buffer : globalUboBuffers) {
    buffer = std::make_unique<Renderer::MemBuffer>(
      this->device,
      sizeof(GlobalUbo),
      1,
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
    );
    buffer->map();
  }

  auto globalSetLayout = Renderer::DescriptorSetLayout::Builder(this->device)
    .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
    .build();

  std::vector<VkDescriptorSet> globalDescriptorSets(Renderer::Swapchain::MAX_FRAMES_IN_FLIGHT);
  for (uint32_t i = 0; i < Renderer::Swapchain::MAX_FRAMES_IN_FLIGHT; ++i) {
    auto bufferInfo = globalUboBuffers[i]->getDescriptorInfo();
    Renderer::DescriptorWriter(*globalSetLayout, *globalDescriptorPool)
      .write(0, &bufferInfo)
      .build(globalDescriptorSets[i]);
  }
  Renderer::Systems::Simple simpleRenderSystem{ this->device, this->renderer.getSwapchainRenderPass(), globalSetLayout->getHandle() };

  this->sceneCamera.setPerspective(glm::radians(50.f), .1f, 10.f);

  auto currentTime = std::chrono::high_resolution_clock::now();
  Input::SetMouseMode(Input::MouseMode::Hidden);
  while (!this->window.shouldClose()) {
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
    const Renderer::FrameInfo frameInfo{
      deltaTime,
      frameIndex,
      cmdBuffer,
      this->sceneCamera,
      globalDescriptorSets[frameIndex]
    };

    // update
    GlobalUbo ubo{};
    ubo.projectionView = this->sceneCamera.getProjectionView();
    globalUboBuffers[frameIndex]->writeTo(&ubo);
    globalUboBuffers[frameIndex]->flush();

    // render
    this->renderer.beginSwapchainRenderPass(cmdBuffer);
    simpleRenderSystem.renderScene(frameInfo, this->scene);
    this->renderer.endSwapchainRenderPass(cmdBuffer);
    this->renderer.endFrame();
    vkDeviceWaitIdle(this->device.getHandle());
  }
  vkDeviceWaitIdle(this->device.getHandle());
}

void App::loadEntities(const std::vector<std::string_view>& modelPaths) {
  uint32_t i = 0;
  for (const auto path : modelPaths) {
    std::shared_ptr<Renderer::Model> cubeModel = Renderer::Model::CreateFromFile(this->device, path);
    auto entity = this->scene.createEntity("Cube");
    entity.addComponent<Components::Mesh>(cubeModel, glm::vec3{1.f, .0f, .2f});
    auto& transform = entity.transform();
    transform.translation = { 0.f, 0.f, -2.5f + 2.f * i++ };
    transform.rotation = { 0.f, 0.f, glm::pi<float>() };
    transform.scale = glm::vec3(1.f);
  }
}

