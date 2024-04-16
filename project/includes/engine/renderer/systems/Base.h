#pragma once

#include <engine/renderer/Device.h>
#include <engine/renderer/Pipeline.h>
#include <engine/renderer/FrameInfo.h>
#include <engine/scene/Scene.h>
#include <engine/renderer/Descriptors.h>

#include <functional>

namespace Scop::Renderer::Systems {
  struct SystemInfo {
    Device& device;
    VkRenderPass renderPass;
    VkDescriptorSetLayout globalDescriptorSetLayout;
  };
  class Base {
  public:
    ~Base();
    Base(const Base&) = delete;
    Base& operator=(const Base&) = delete;

    virtual void render(const FrameInfo& frameInfo, Scene& scene) = 0;
    virtual void update(const FrameInfo& frameInfo) = 0;
  protected:
    Base(
      const SystemInfo& dependencies,
      const std::string_view vertFilePath,
      const std::string_view fragFilePath
    );
    void init(const SystemInfo& dependencies, std::function<void(Pipeline::ConfigInfo&)> pipelineCb = nullptr);
    virtual void createPipelineLayout(VkDescriptorSetLayout globalDescriptorSetLayout) = 0;
    virtual void createPipeline(
      VkRenderPass renderPass,
      std::function<void(Pipeline::ConfigInfo&)> cb = nullptr
    );

    Device& device;
    std::unique_ptr<Pipeline> pipeline;
    VkPipelineLayout pipelineLayout;
  private:
    const std::string_view vertFilePath;
    const std::string_view fragFilePath;
  };
}