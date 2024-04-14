#include "App.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>
#include <stdexcept>
#include <array>
#include <chrono>

#include <engine/entity/components/Mesh.h>

using Scop::App;
using Scop::Entity;


App::App() {
  this->loadEntities();
}

App::~App() {}

void App::run() {
  while (!this->window.shouldClose()) {
    this->window.pollEvents();
    auto cmdBuffer = this->renderer.beginFrame();
    if (!cmdBuffer)
      continue;
    this->renderer.beginSwapchainRenderPass(cmdBuffer);
    this->simpleRenderSystem.renderScene(cmdBuffer, this->scene);
    this->renderer.endSwapchainRenderPass(cmdBuffer);
    this->renderer.endFrame();
    vkDeviceWaitIdle(this->device.getHandle());
  }
  vkDeviceWaitIdle(this->device.getHandle());
}

void App::loadEntities() {
  std::vector<Renderer::Model::Vertex> vertices{
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
  };
  auto model = std::make_shared<Renderer::Model>(this->device, vertices);

  std::vector<glm::vec3> colors{
    {1.f, .7f, .73f},
    {1.f, .87f, .73f},
    {1.f, 1.f, .73f},
    {.73f, 1.f, .8f},
    {.73, .88f, 1.f}
  };
  for (auto& color : colors) {
    color = glm::pow(color, glm::vec3{ 2.2f });
  }
  for (int i = 0; i < 40; i++) {
    auto triangle = this->scene.createEntity("Triangle" + std::to_string(i));
    triangle.addComponent<Components::Mesh>(model, colors[i % colors.size()]);
    auto& transform = triangle.transform();
    transform.scale = glm::vec2(.5f) + i * 0.025f;
    transform.rotation = i * glm::pi<float>() * .025f;
  }
}

