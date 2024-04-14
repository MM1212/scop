#include "engine/renderer/systems/Simple.h"
#include <engine/entity/components/Mesh.h>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

using Scop::Renderer::Systems::Simple;

struct SimplePushConstantData {
  glm::mat2 transform{ 1.0f };
  glm::vec2 offset;
  alignas(16) glm::vec3 color;
};

Simple::Simple(Device& device, VkRenderPass renderPass)
  : device(device) {
  this->createPipelineLayout();
  this->createPipeline(renderPass);
}

Simple::~Simple() {
  vkDestroyPipelineLayout(this->device.getHandle(), this->pipelineLayout, nullptr);
}

void Simple::createPipelineLayout() {
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

void Simple::createPipeline(VkRenderPass renderPass) {
  assert(this->pipelineLayout != VK_NULL_HANDLE && "pipeline layout is null");
  Pipeline::ConfigInfo pipelineConfig{};

  Pipeline::SetupDefaultConfigInfo(pipelineConfig);
  pipelineConfig.renderPass = renderPass;
  pipelineConfig.pipelineLayout = this->pipelineLayout;

  this->pipeline = std::make_unique<Pipeline>(
    this->device,
    SHADERS_PATH"simple.vert.spv",
    SHADERS_PATH"simple.frag.spv",
    pipelineConfig
  );
}

void Simple::renderScene(VkCommandBuffer commandBuffer, Scene& scene) {
  this->pipeline->bind(commandBuffer);
  auto group = scene.groupEntitiesWith<Components::Mesh, Components::Transform>();
  uint32_t i = 0;
  for (auto entity : group) {
    auto [mesh, transform] = group.get<Components::Mesh, Components::Transform>(entity);

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