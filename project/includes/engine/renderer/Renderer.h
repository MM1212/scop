#pragma once

#include <engine/Window.h>
#include <engine/renderer/Device.h>
#include <engine/renderer/Swapchain.h>
#include <engine/renderer/Model.h>
#include <memory>
#include <vector>
#include <cassert>

namespace Scop::Renderer {
  class Renderer {
  public:
    Renderer(Window& window, Device& device);
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    VkRenderPass getSwapchainRenderPass() const { return this->swapchain->getRenderPass(); }
    bool isFrameInProgress() const { return this->isFrameStarted; }
    VkCommandBuffer getCurrentCommandBuffer() const {
      assert(this->isFrameStarted && "Cannot get command buffer when frame not in progress.");
      return this->commandBuffers[this->currentFrameIndex];
    }
    uint32_t getFrameIndex() const {
      assert(this->isFrameStarted && "Cannot get frame index when frame not in progress.");
      return this->currentFrameIndex;
    }

    VkCommandBuffer beginFrame();
    void endFrame();
    void beginSwapchainRenderPass(VkCommandBuffer commandBuffer);
    void endSwapchainRenderPass(VkCommandBuffer commandBuffer);
  private:
    void createCommandBuffers();
    void freeCommandBuffers();
    void recreateSwapchain();

    Window& window;
    Device& device;
    std::unique_ptr<Swapchain> swapchain;
    std::vector<VkCommandBuffer> commandBuffers;
    uint32_t currentImageIndex = 0;
    uint32_t currentFrameIndex = 0;
    bool isFrameStarted = false;
  };
}