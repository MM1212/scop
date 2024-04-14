#include "App.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>
#include <stdexcept>
#include <array>
#include <chrono>

#include <engine/entity/components/Mesh.h>

using Scop::App;
using Scop::Entity;

struct SimplePushConstantData {
  glm::mat2 transform{ 1.0f };
  glm::vec2 offset;
  alignas(16) glm::vec3 color;
};

App::App() {
  this->loadEntities();
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

void App::loadEntities() {
  std::vector<Renderer::Model::Vertex> vertices{
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
  };
  auto model = std::make_shared<Renderer::Model>(this->device, vertices);

  // auto triangle = this->createEntity("Triangle");
  // triangle.addComponent<Components::Mesh>(model, glm::vec3{ .1f, .8f, .1f });
  // auto& transform = triangle.transform();
  // transform.translation.x = .2f;
  // transform.scale = { 2.f, .5f };
  // transform.rotation = glm::radians(90.f);

  std::vector<glm::vec3> colors{
    {1.f, .7f, .73f},
    {1.f, .87f, .73f},
    {1.f, 1.f, .73f},
    {.73f, 1.f, .8f},
    {.73, .88f, 1.f}
  };
  for (auto& color : colors) {
    color = glm::pow(color, glm::vec3{ 2.2f });
  }
  for (int i = 0; i < 40; i++) {
    auto triangle = this->createEntity("Triangle" + std::to_string(i));
    triangle.addComponent<Components::Mesh>(model, colors[i % colors.size()]);
    auto& transform = triangle.transform();
    transform.scale = glm::vec2(.5f) + i * 0.025f;
    transform.rotation = i * glm::pi<float>() * .025f;
  }
}

void App::createPipelineLayout() {
  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(SimplePushConstantData);

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pSetLayouts = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

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
  clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
  clearValues[1].depthStencil = { 1.0f, 0 };

  renderPassInfo.clearValueCount = clearValues.size();
  renderPassInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(this->commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport{};
  VkRect2D scissor{ {0,0}, this->swapchain->getSwapChainExtent() };
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = this->swapchain->getSwapChainExtent().width;
  viewport.height = this->swapchain->getSwapChainExtent().height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  vkCmdSetViewport(this->commandBuffers[imageIndex], 0, 1, &viewport);
  vkCmdSetScissor(this->commandBuffers[imageIndex], 0, 1, &scissor);

  this->pipeline->bind(this->commandBuffers[imageIndex]);
  this->renderEntities(this->commandBuffers[imageIndex]);
  vkCmdEndRenderPass(this->commandBuffers[imageIndex]);
  if (vkEndCommandBuffer(this->commandBuffers[imageIndex]) != VK_SUCCESS)
    throw std::runtime_error("Failed to record command buffer");
}

void App::renderEntities(VkCommandBuffer commandBuffer) {
  auto view = this->entityRegistry.view<Components::Transform, Components::Mesh>();
  uint32_t i = 0;
  for (auto entity : view) {

    auto [mesh, transform] = view.get<Components::Mesh, Components::Transform>(entity);

    // transform.rotation = glm::mod(transform.rotation + 0.001f, glm::two_pi<float>());
    // transform.scale.x = glm::abs(glm::sin(transform.rotation)) + 0.5f;
    // transform.scale.y = transform.scale.x;
    transform.rotation = glm::mod<float>(transform.rotation + 0.001f * (++i), 2.f * glm::pi<float>());

    SimplePushConstantData data;
    data.transform = static_cast<glm::mat2>(transform);
    data.offset = transform.translation;
    data.color = mesh.color;
    vkCmdPushConstants(
      commandBuffer,
      this->pipelineLayout,
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
      0,
      sizeof(SimplePushConstantData),
      &data
    );
    mesh.model->bind(commandBuffer);
    mesh.model->draw(commandBuffer);
  }
}

void App::drawFrame() {
  static auto startTime = std::chrono::high_resolution_clock::now();

  auto currentTime = std::chrono::high_resolution_clock::now();

  float deltaTime = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - startTime).count();
  startTime = currentTime;
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

Entity App::createEntity(const std::string_view tag) {
  Entity entity{ this->entityRegistry.create(), this->entityRegistry };
  uint64_t id = Components::ID::NewRandomId();
  if (tag.empty())
    entity.addComponent<Components::ID>("Entity " + std::to_string(id), id);
  else
    entity.addComponent<Components::ID>(tag, id);
  entity.addComponent<Components::Transform>();
  return entity;
}