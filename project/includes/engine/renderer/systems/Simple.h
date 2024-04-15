#pragma once

#include <engine/renderer/Device.h>
#include <engine/renderer/Renderer.h>
#include <engine/renderer/Pipeline.h>
#include <memory>
#include <engine/scene/Scene.h>
#include <engine/renderer/FrameInfo.h>

namespace Scop::Renderer::Systems {
  class Simple {
  public:
    Simple(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalDescriptorSetLayout);
    ~Simple();
    Simple(const Simple&) = delete;
    Simple& operator=(const Simple&) = delete;

    void renderScene(const FrameInfo& frameInfo, Scene& scene);
  private:
    void createPipelineLayout(VkDescriptorSetLayout globalDescriptorSetLayout);
    void createPipeline(VkRenderPass renderPass);

    Device& device;
    std::unique_ptr<Pipeline> pipeline;
    VkPipelineLayout pipelineLayout;
  };
}