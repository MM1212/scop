#pragma once

#include <engine/renderer/Device.h>
#include <engine/renderer/Renderer.h>
#include <engine/renderer/Pipeline.h>
#include <memory>
#include <engine/Scene.h>

namespace Scop::Renderer::Systems {
  class Simple {
  public:
    Simple(Device& device, VkRenderPass renderPass);
    ~Simple();
    Simple(const Simple&) = delete;
    Simple& operator=(const Simple&) = delete;

    void renderScene(VkCommandBuffer commandBuffer, Scene& scene);
  private:
    void createPipelineLayout();
    void createPipeline(VkRenderPass renderPass);

    Device& device;
    std::unique_ptr<Pipeline> pipeline;
    VkPipelineLayout pipelineLayout;
  };
}