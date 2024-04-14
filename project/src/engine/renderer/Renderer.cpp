#include "engine/renderer/Renderer.h"

#include <stdexcept>
#include <array>

using Scop::Renderer::Renderer;

Renderer::Renderer(Window& window, Device& device)
  : window{ window }, device{ device } {
  this->recreateSwapchain();
  this->createCommandBuffers();
}

Renderer::~Renderer() {
  this->freeCommandBuffers();
}

void Renderer::recreateSwapchain() {
  auto extent = this->window.getExtent();
  while (extent.width == 0 || extent.height == 0) {
    extent = this->window.getExtent();
    glfwWaitEvents();
  }
  vkDeviceWaitIdle(this->device.getHandle());
  if (!this->swapchain)
    this->swapchain = std::make_unique<Swapchain>(this->device, extent);
  else {
    this->swapchain = std::make_unique<Swapchain>(this->device, extent, std::move(this->swapchain));
    if (this->swapchain->getImageCount() != this->commandBuffers.size()) {
      this->freeCommandBuffers();
      this->createCommandBuffers();
    }
  }
}

void Renderer::createCommandBuffers() {
  this->commandBuffers.resize(this->swapchain->getImageCount());

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = this->device.getCommandPool();
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = this->commandBuffers.size();

  if (vkAllocateCommandBuffers(this->device.getHandle(), &allocInfo, this->commandBuffers.data()) != VK_SUCCESS)
    throw std::runtime_error("Failed to allocate command buffers");
}

void Renderer::freeCommandBuffers() {
  vkFreeCommandBuffers(
    this->device.getHandle(),
    this->device.getCommandPool(),
    this->commandBuffers.size(),
    this->commandBuffers.data()
  );
  this->commandBuffers.clear();
}

VkCommandBuffer Renderer::beginFrame() {
  assert(!this->isFrameStarted && "Cannot call beginFrame while frame is in progress");
  auto result = this->swapchain->acquireNextImage(&this->currentImageIndex);
  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR && result != VK_ERROR_OUT_OF_DATE_KHR) {
    throw std::runtime_error("Failed to acquire next image");
  }
  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    this->recreateSwapchain();
    return nullptr;
  }
  this->isFrameStarted = true;

  auto commandBuffer = this->getCurrentCommandBuffer();
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    throw std::runtime_error("Failed to begin recording command buffer");
  return commandBuffer;
}
void Renderer::endFrame() {
  assert(this->isFrameStarted && "Cannot call endFrame while frame is not in progress");
  auto cmdBuffer = this->getCurrentCommandBuffer();
  if (vkEndCommandBuffer(cmdBuffer) != VK_SUCCESS)
    throw std::runtime_error("Failed to record command buffer");
  VkResult result = this->swapchain->submitCommandBuffers(&cmdBuffer, &this->currentImageIndex);
  if (
    result == VK_ERROR_OUT_OF_DATE_KHR ||
    result == VK_SUBOPTIMAL_KHR ||
    this->window.wasResized()
    ) {
    this->window.resetResizedFlag();
    this->recreateSwapchain();
  }
  else if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to submit command buffer");
  }
  this->isFrameStarted = false;
}
void Renderer::beginSwapchainRenderPass(VkCommandBuffer commandBuffer) {
  assert(this->isFrameStarted && "Cannot begin render pass when frame is not in progress.");
  assert(commandBuffer == this->getCurrentCommandBuffer() && "Can only begin render pass for command buffer which is being recorded.");
  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = this->swapchain->getRenderPass();
  renderPassInfo.framebuffer = this->swapchain->getFrameBuffer(this->currentImageIndex);
  renderPassInfo.renderArea.offset = { 0, 0 };
  renderPassInfo.renderArea.extent = this->swapchain->getSwapChainExtent();

  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
  clearValues[1].depthStencil = { 1.0f, 0 };

  renderPassInfo.clearValueCount = clearValues.size();
  renderPassInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport{};
  VkRect2D scissor{ {0,0}, this->swapchain->getSwapChainExtent() };
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = this->swapchain->getSwapChainExtent().width;
  viewport.height = this->swapchain->getSwapChainExtent().height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}
void Renderer::endSwapchainRenderPass(VkCommandBuffer commandBuffer) {
  assert(this->isFrameStarted && "Cannot end render pass when frame is not in progress.");
  assert(commandBuffer == this->getCurrentCommandBuffer() && "Can only end render pass for command buffer which is being recorded.");
  vkCmdEndRenderPass(commandBuffer);
}