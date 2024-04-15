#include "App.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>
#include <stdexcept>
#include <array>
#include <chrono>

#include <engine/entity/components/Mesh.h>
#include <engine/entity/components/RigidBody2D.h>

using namespace Scop;



App::App() {
  this->loadEntities();
}

App::~App() {}

void App::run() {
  Renderer::Systems::Simple simpleRenderSystem{ this->device, this->renderer.getSwapchainRenderPass() };
  while (!this->window.shouldClose()) {
    this->window.pollEvents();
    auto cmdBuffer = this->renderer.beginFrame();
    if (!cmdBuffer)
      continue;
    // render
    this->renderer.beginSwapchainRenderPass(cmdBuffer);
    simpleRenderSystem.renderScene(cmdBuffer, this->scene);
    this->renderer.endSwapchainRenderPass(cmdBuffer);
    this->renderer.endFrame();
    vkDeviceWaitIdle(this->device.getHandle());
  }
  vkDeviceWaitIdle(this->device.getHandle());
}

static std::unique_ptr<Renderer::Model> CreateCubeModel(Renderer::Device& device, glm::vec3 offset) {
  std::vector<Renderer::Model::Vertex> vertices{

    // left face (white)
    {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
    {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
    {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
    {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
    {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
    {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

    // right face (yellow)
    {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
    {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
    {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
    {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
    {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
    {{.5f, .5f, .5f}, {.8f, .8f, .1f}},

    // top face (orange, remember y axis points down)
    {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
    {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
    {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
    {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
    {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
    {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

    // bottom face (red)
    {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
    {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
    {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
    {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
    {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
    {{.5f, .5f, .5f}, {.8f, .1f, .1f}},

    // nose face (blue)
    {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
    {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
    {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
    {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
    {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
    {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

    // tail face (green)
    {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
    {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
    {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
    {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
    {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
    {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

  };
  for (auto& v : vertices) {
    v.position += offset;
  }
  return std::make_unique<Renderer::Model>(device, vertices);
}

void App::loadEntities() {
  std::shared_ptr<Renderer::Model> cubeModel = CreateCubeModel(this->device, { 0.f, 0.f, 0.f });
  auto cube = this->scene.createEntity("Cube");
  cube.addComponent<Components::Mesh>(cubeModel);
  auto& transform = cube.transform();
  transform.translation = { 0.f, 0.f, .5f };
  transform.scale = { .5f, .5f, .5f };
}

