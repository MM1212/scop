#include "engine/renderer/systems/Base.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

using Scop::Renderer::Systems::Base;

Base::Base(
  const SystemInfo& deps,
  const std::string_view vertFilePath,
  const std::string_view fragFilePath
) : device(deps.device), vertFilePath(vertFilePath), fragFilePath(fragFilePath) {}

void Base::init(const SystemInfo& deps, std::function<void(Pipeline::ConfigInfo&)> pipelineCb) {
  this->createPipelineLayout(deps.globalDescriptorSetLayout);
  this->createPipeline(deps.renderPass, pipelineCb);
}

Base::~Base() {
  vkDestroyPipelineLayout(this->device.getHandle(), this->pipelineLayout, nullptr);
}

void Base::createPipelineLayout(VkDescriptorSetLayout globalDescriptorSetLayout) {
  std::vector<VkDescriptorSetLayout> layouts = { globalDescriptorSetLayout };

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = layouts.size();
  pipelineLayoutInfo.pSetLayouts = layouts.data();
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.pPushConstantRanges = nullptr;

  if (vkCreatePipelineLayout(this->device.getHandle(), &pipelineLayoutInfo, nullptr, &this->pipelineLayout) != VK_SUCCESS)
    throw std::runtime_error("Failed to create pipeline layout");
}

void Base::createPipeline(
  VkRenderPass renderPass,
  std::function<void(Pipeline::ConfigInfo&)> cb
) {
  assert(this->pipelineLayout != VK_NULL_HANDLE && "pipeline layout is null");
  Pipeline::ConfigInfo pipelineConfig{};

  Pipeline::SetupDefaultConfigInfo(pipelineConfig);
  pipelineConfig.renderPass = renderPass;
  pipelineConfig.pipelineLayout = this->pipelineLayout;
  if (cb)
    cb(pipelineConfig);

  this->pipeline = std::make_unique<Pipeline>(
    this->device,
    this->vertFilePath,
    this->fragFilePath,
    pipelineConfig
  );
}
