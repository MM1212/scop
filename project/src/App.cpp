#include "App.h"

#include <iostream>
#include <stdexcept>
#include <array>

using Scop::App;

App::App() {
  this->loadModels();
  this->createPipelineLayout();
  this->recreateSwapchain();
  this->createCommandBuffers();
}

App::~App() {
  vkDestroyPipelineLayout(this->device.getHandle(), this->pipelineLayout, nullptr);
}

void App::run() {
  while (!this->window.shouldClose()) {
    this->window.pollEvents();
    this->drawFrame();
    vkDeviceWaitIdle(this->device.getHandle());
  }
  vkDeviceWaitIdle(this->device.getHandle());
}

void App::loadModels() {
  std::vector<Renderer::Model::Vertex> vertices{
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
  };
  this->model = std::make_unique<Renderer::Model>(this->device, vertices);
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
  assert(this->swapchain && "swapchain is null - create pipeline before swapchain");
  assert(this->pipelineLayout != VK_NULL_HANDLE && "pipeline layout is null");
  Renderer::Pipeline::ConfigInfo pipelineConfig{};

  Renderer::Pipeline::SetupDefaultConfigInfo(pipelineConfig);
  pipelineConfig.renderPass = this->swapchain->getRenderPass();
  pipelineConfig.pipelineLayout = this->pipelineLayout;

  this->pipeline = std::make_unique<Renderer::Pipeline>(
    this->device,
    SHADERS_PATH"simple.vert.spv",
    SHADERS_PATH"simple.frag.spv",
    pipelineConfig
  );
}

void App::recreateSwapchain() {
  auto extent = this->window.getExtent();
  while (extent.width == 0 || extent.height == 0) {
    extent = this->window.getExtent();
    glfwWaitEvents();
  }
  vkDeviceWaitIdle(this->device.getHandle());
  if (!this->swapchain)
    this->swapchain = std::make_unique<Renderer::Swapchain>(this->device, extent);
  else {
    this->swapchain = std::make_unique<Renderer::Swapchain>(this->device, extent, std::move(this->swapchain));
    if (this->swapchain->getImageCount() != this->commandBuffers.size()) {
      this->freeCommandBuffers();
      this->createCommandBuffers();
    }
  }
  this->createPipeline();
}

void App::createCommandBuffers() {
  this->commandBuffers.resize(this->swapchain->getImageCount());

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = this->device.getCommandPool();
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = this->commandBuffers.size();

  if (vkAllocateCommandBuffers(this->device.getHandle(), &allocInfo, this->commandBuffers.data()) != VK_SUCCESS)
    throw std::runtime_error("Failed to allocate command buffers");
  std::cout << "Allocated " << this->commandBuffers.size() << " command buffers" << std::endl;
}

void App::freeCommandBuffers() {
  vkFreeCommandBuffers(
    this->device.getHandle(),
    this->device.getCommandPool(),
    this->commandBuffers.size(),
    this->commandBuffers.data()
  );
  this->commandBuffers.clear();
}

void App::recordCommandBuffer(uint32_t imageIndex) {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(this->commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
    throw std::runtime_error("Failed to begin recording command buffer");

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = this->swapchain->getRenderPass();
  renderPassInfo.framebuffer = this->swapchain->getFrameBuffer(imageIndex);
  renderPassInfo.renderArea.offset = { 0, 0 };
  renderPassInfo.renderArea.extent = this->swapchain->getSwapChainExtent();

  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
  clearValues[1].depthStencil = { 1.0f, 0 };

  renderPassInfo.clearValueCount = clearValues.size();
  renderPassInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(this->commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport{};
  VkRect2D scissor{{0,0}, this->swapchain->getSwapChainExtent()};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = this->swapchain->getSwapChainExtent().width;
  viewport.height = this->swapchain->getSwapChainExtent().height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  vkCmdSetViewport(this->commandBuffers[imageIndex], 0, 1, &viewport);
  vkCmdSetScissor(this->commandBuffers[imageIndex], 0, 1, &scissor);

  this->pipeline->bind(this->commandBuffers[imageIndex]);
  this->model->bind(this->commandBuffers[imageIndex]);
  this->model->draw(this->commandBuffers[imageIndex]);
  vkCmdEndRenderPass(this->commandBuffers[imageIndex]);
  if (vkEndCommandBuffer(this->commandBuffers[imageIndex]) != VK_SUCCESS)
    throw std::runtime_error("Failed to record command buffer");
}

void App::drawFrame() {
  uint32_t imageIndex;
  auto result = this->swapchain->acquireNextImage(&imageIndex);
  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR && result != VK_ERROR_OUT_OF_DATE_KHR) {
    throw std::runtime_error("Failed to acquire next image");
  }
  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    this->recreateSwapchain();
    return;
  }

  this->recordCommandBuffer(imageIndex);
  result = this->swapchain->submitCommandBuffers(&this->commandBuffers[imageIndex], &imageIndex);
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
}