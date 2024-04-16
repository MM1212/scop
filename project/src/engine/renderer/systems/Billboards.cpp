#include "engine/renderer/systems/Billboards.h"
#include <engine/scene/components/Billboard.h>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <iostream>

using Scop::Renderer::Systems::Billboards;

struct BillboardsPushConstantData {
  glm::vec4 position;
  glm::vec4 color;
  float width;
  float height;
  bool rounded;
  uint8_t _pad0[4];
};

Billboards::Billboards(const SystemInfo& deps) : Base(
  deps,
  SHADERS_PATH"billboard.vert.spv",
  SHADERS_PATH"billboard.frag.spv"
) {
  this->init(deps, [](Pipeline::ConfigInfo& config) {
    config.attributeDescriptions.clear();
    config.bindingDescriptions.clear();
    });
}

void Billboards::createPipelineLayout(VkDescriptorSetLayout globalDescriptorSetLayout) {
  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(BillboardsPushConstantData);

  std::vector<VkDescriptorSetLayout> layouts = { globalDescriptorSetLayout };

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = layouts.size();
  pipelineLayoutInfo.pSetLayouts = layouts.data();
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

  if (vkCreatePipelineLayout(this->device.getHandle(), &pipelineLayoutInfo, nullptr, &this->pipelineLayout) != VK_SUCCESS)
    throw std::runtime_error("Failed to create pipeline layout");
}

void Billboards::render(const FrameInfo& frameInfo, Scene& scene) {
  this->pipeline->bind(frameInfo.commandBuffer);

  vkCmdBindDescriptorSets(
    frameInfo.commandBuffer,
    VK_PIPELINE_BIND_POINT_GRAPHICS,
    this->pipelineLayout,
    0, 1, &frameInfo.globalDescriptorSet,
    0, nullptr
  );

  (void)scene;

  auto group = scene.viewEntitiesWith<Components::Billboard, Components::Transform>();
  for (auto entity : group) {
    auto [billboard, transform] = group.get<Components::Billboard, Components::Transform>(entity);
    BillboardsPushConstantData data;
    data.position = glm::vec4(transform.translation, 1.0f);
    data.color = billboard.color;
    data.width = billboard.size.x;
    data.height = billboard.size.y;
    data.rounded = billboard.rounded;

    vkCmdPushConstants(
      frameInfo.commandBuffer,
      this->pipelineLayout,
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
      0,
      sizeof(BillboardsPushConstantData),
      &data
    );
    vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
  }
}