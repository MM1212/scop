#pragma once

#include "Device.h"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <string>
#include <vector>
#include <memory>

namespace Scop::Renderer {

  class Swapchain {
  public:
    static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

    Swapchain(Device& deviceRef, VkExtent2D windowExtent);
    Swapchain(Device& deviceRef, VkExtent2D windowExtent, std::shared_ptr<Swapchain> previous);
    ~Swapchain();

    Swapchain(const Swapchain&) = delete;
    void operator=(const Swapchain&) = delete;

    VkFramebuffer getFrameBuffer(int index) const { return this->swapChainFramebuffers[index]; }
    VkRenderPass getRenderPass() const { return this->renderPass; }
    VkImageView getImageView(int index) const { return this->swapChainImageViews[index]; }
    size_t getImageCount() const { return this->swapChainImages.size(); }
    VkFormat getImageFormat() const { return this->swapChainImageFormat; }
    VkExtent2D getExtent() const { return this->swapChainExtent; }
    float getAspectRation() const { return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height); }
    uint32_t width() const { return this->swapChainExtent.width; }
    uint32_t height() const { return this->swapChainExtent.height; }

    float extentAspectRatio() {
      return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
    }
    VkFormat findDepthFormat();

    VkResult acquireNextImage(uint32_t* imageIndex);
    VkResult submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

    bool compareSwapFormats(const Swapchain& swapchain) const {
      return this->swapChainImageFormat == swapchain.swapChainImageFormat &&
        this->swapChainDepthFormat == swapchain.swapChainDepthFormat;
    }
  private:
    void init();
    void createSwapChain();
    void createImageViews();
    void createDepthResources();
    void createRenderPass();
    void createFramebuffers();
    void createSyncObjects();

    // Helper functions
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    VkFormat swapChainImageFormat;
    VkFormat swapChainDepthFormat;
    VkExtent2D swapChainExtent;

    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkRenderPass renderPass;

    std::vector<VkImage> depthImages;
    std::vector<VkDeviceMemory> depthImageMemorys;
    std::vector<VkImageView> depthImageViews;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;

    Device& device;
    VkExtent2D windowExtent;

    VkSwapchainKHR swapChain;
    std::shared_ptr<Swapchain> oldSwapchain;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame = 0;
  };
}