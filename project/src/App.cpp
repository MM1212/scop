#include "App.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>
#include <stdexcept>
#include <array>
#include <chrono>

#include <engine/scene/components/Mesh.h>
#include <engine/scene/components/RigidBody2D.h>
#include <engine/input/Input.h>

using namespace Scop;

App* App::instance = nullptr;

App::App() {
  App::instance = this;
}
App::~App() {}

void App::run() {
  Renderer::Systems::Simple simpleRenderSystem{ this->device, this->renderer.getSwapchainRenderPass() };

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
    // render
    this->renderer.beginSwapchainRenderPass(cmdBuffer);
    simpleRenderSystem.renderScene(cmdBuffer, this->scene, this->sceneCamera);
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
    entity.addComponent<Components::Mesh>(cubeModel);
    auto& transform = entity.transform();
    transform.translation = { 0.f, 0.f, -2.5f + 2.f * i++};
    transform.scale = { 3.f, 1.5f, 3.f };
  }
}

