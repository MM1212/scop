#pragma once

#include <engine/Window.h>
#include <engine/scene/Entity.h>
#include <engine/renderer/Renderer.h>
#include <engine/scene/Scene.h>
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
    const Window& getWindow() const { return this->window; }
    const Renderer::Device& getDevice() const { return this->device; }
    const Renderer::Renderer& getRenderer() const { return this->renderer; }
    Scene& getActiveScene() { return this->scene; }
    const Scene& getActiveScene() const { return this->scene; }
    SceneCamera& getSceneCamera() { return this->sceneCamera; }
    const SceneCamera& getSceneCamera() const { return this->sceneCamera; }

    static App& Get() { return *instance; }
  private:
    void loadEntities();

    Window window{ WINDOW_SIZE, "Scop" };
    Renderer::Device device{ window };
    Renderer::Renderer renderer{ window, device };
    SceneCamera sceneCamera{};
    Scene scene;
  private:
    static App* instance;
  };
}