#pragma once

#include <engine/Window.h>
#include <engine/renderer/Pipeline.h>
#include <engine/renderer/Device.h>
#include <engine/renderer/Swapchain.h>

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
    void createPipelineLayout();
    void createPipeline();
    void createCommandBuffers();
    void drawFrame();

    Window window{ WINDOW_SIZE, "Scop" };
    Renderer::Device device{ window };
    Renderer::Swapchain swapchain{ device, window.getExtent() };
    // Renderer::Pipeline pipeline{
    //   device,
    //   SHADERS_PATH"simple.vert.spv",
    //   SHADERS_PATH"simple.frag.spv",
    //   Renderer::Pipeline::CreateDefaultConfigInfo(WINDOW_SIZE)
    // };
    std::unique_ptr<Renderer::Pipeline> pipeline;
    VkPipelineLayout pipelineLayout;
    std::vector<VkCommandBuffer> commandBuffers;
  };
}