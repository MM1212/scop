#include "engine/renderer/systems/Simple.h"
#include <engine/scene/components/Mesh.h>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

using Scop::Renderer::Systems::Simple;

struct BillboardsPushConstantData {
  glm::mat4 modelMatrix{ 1.0f };
  glm::mat4 normalMatrix{ 1.0f };
};

Simple::Simple(
  const SystemInfo& deps
) : Base(
  deps,
  SHADERS_PATH"simple.vert.spv",
  SHADERS_PATH"simple.frag.spv"
) {
  this->init(deps);
}

void Simple::createPipelineLayout(VkDescriptorSetLayout globalDescriptorSetLayout) {
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

void Simple::render(const FrameInfo& frameInfo, Scene& scene) {
  this->pipeline->bind(frameInfo.commandBuffer);

  vkCmdBindDescriptorSets(
    frameInfo.commandBuffer,
    VK_PIPELINE_BIND_POINT_GRAPHICS,
    this->pipelineLayout,
    0, 1, &frameInfo.globalDescriptorSet,
    0, nullptr
  );

  auto group = scene.viewEntitiesWith<Components::Mesh, Components::Transform>();
  for (auto entity : group) {
    auto [mesh, transform] = group.get<Components::Mesh, Components::Transform>(entity);
    BillboardsPushConstantData data;
    auto modelMatrix = static_cast<glm::mat4>(transform);
    data.modelMatrix = modelMatrix;
    data.normalMatrix = transform.computeNormalMatrix();

    vkCmdPushConstants(
      frameInfo.commandBuffer,
      this->pipelineLayout,
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
      0,
      sizeof(BillboardsPushConstantData),
      &data
    );
    mesh.model->bind(frameInfo.commandBuffer);
    mesh.model->draw(frameInfo.commandBuffer);
  }
}