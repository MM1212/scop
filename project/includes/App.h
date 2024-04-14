#pragma once

#include <engine/Window.h>
#include <engine/renderer/Pipeline.h>
#include <engine/renderer/Device.h>
#include <engine/renderer/Swapchain.h>
#include <engine/renderer/Model.hpp>

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
    void loadModels();
    void createPipelineLayout();
    void createPipeline();
    void createCommandBuffers();
    void freeCommandBuffers();
    void drawFrame();
    void recreateSwapchain();
    void recordCommandBuffer(uint32_t imageIndex);

    Window window{ WINDOW_SIZE, "Scop" };
    Renderer::Device device{ window };
    std::unique_ptr<Renderer::Swapchain> swapchain;
    std::unique_ptr<Renderer::Pipeline> pipeline;
    VkPipelineLayout pipelineLayout;
    std::vector<VkCommandBuffer> commandBuffers;
    std::unique_ptr<Renderer::Model> model;
  };
}