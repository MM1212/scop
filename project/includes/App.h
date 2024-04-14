#pragma once

#include <engine/Window.h>
#include <engine/entity/Entity.h>
#include <engine/renderer/Renderer.h>
#include <engine/Scene.h>
#include <engine/renderer/systems/Simple.h>
#include <memory>
#include <vector>

namespace Scop {
  class App {
  public:
    static constexpr glm::uvec2 WINDOW_SIZE = { 800, 600 };

    App();
    ~App();
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    void run();
  private:
    void loadEntities();

    Window window{ WINDOW_SIZE, "Scop" };
    Renderer::Device device{ window };
    Renderer::Renderer renderer{ window, device };
    Renderer::Systems::Simple simpleRenderSystem{ device, renderer.getSwapchainRenderPass() };
    Scene scene;
  };
}