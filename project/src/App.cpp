#include "App.h"

#include <stdexcept>
#include <array>
using Scop::App;

App::App() {
  this->createPipelineLayout();
  this->createPipeline();
  this->createCommandBuffers();
}

App::~App() {
  vkDestroyPipelineLayout(this->device.getHandle(), this->pipelineLayout, nullptr);
}

void App::run() {
  while (!this->window.shouldClose()) {
    this->window.pollEvents();
    this->drawFrame();
  }
  vkDeviceWaitIdle(this->device.getHandle());
}

void App::createPipelineLayout() {
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pSetLayouts = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.pPushConstantRanges = nullptr;

  if (vkCreatePipelineLayout(this->device.getHandle(), &pipelineLayoutInfo, nullptr, &this->pipelineLayout) != VK_SUCCESS)
    throw std::runtime_error("Failed to create pipeline layout");
}

void App::createPipeline() {
  auto pipelineConfig = Renderer::Pipeline::CreateDefaultConfigInfo({ this->swapchain.width(), this->swapchain.height() });
  pipelineConfig.renderPass = this->swapchain.getRenderPass();
  pipelineConfig.pipelineLayout = this->pipelineLayout;

  this->pipeline = std::make_unique<Renderer::Pipeline>(
    this->device,
    SHADERS_PATH"simple.vert.spv",
    SHADERS_PATH"simple.frag.spv",
    pipelineConfig
  );
}

void App::createCommandBuffers() {
  this->commandBuffers.resize(this->swapchain.getImageCount());

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = this->device.getCommandPool();
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = this->commandBuffers.size();

  if (vkAllocateCommandBuffers(this->device.getHandle(), &allocInfo, this->commandBuffers.data()) != VK_SUCCESS)
    throw std::runtime_error("Failed to allocate command buffers");
  
  for (size_t i = 0; i < this->commandBuffers.size(); i++) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(this->commandBuffers[i], &beginInfo) != VK_SUCCESS)
      throw std::runtime_error("Failed to begin recording command buffer");
    
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = this->swapchain.getRenderPass();
    renderPassInfo.framebuffer = this->swapchain.getFrameBuffer(i);
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = this->swapchain.getSwapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
    clearValues[1].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = clearValues.size();
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(this->commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    this->pipeline->bind(this->commandBuffers[i]);
    vkCmdDraw(this->commandBuffers[i], 3, 1, 0, 0);
    vkCmdEndRenderPass(this->commandBuffers[i]);
    if (vkEndCommandBuffer(this->commandBuffers[i]) != VK_SUCCESS)
      throw std::runtime_error("Failed to record command buffer");
  }

}
void App::drawFrame() {
  uint32_t imageIndex;
  auto result = this->swapchain.acquireNextImage(&imageIndex);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to acquire next image");
  }

  result = this->swapchain.submitCommandBuffers(&this->commandBuffers[imageIndex], &imageIndex);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to submit command buffer");
  }
}