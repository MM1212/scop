#pragma once

#include <engine/renderer/Device.h>
#include <engine/renderer/Renderer.h>
#include <engine/renderer/Pipeline.h>
#include <memory>
#include <engine/scene/Scene.h>
#include <engine/renderer/FrameInfo.h>

#include "Base.h"

namespace Scop::Renderer::Systems {
  class Simple : public Base {
  public:
    Simple(const SystemInfo& dependencies);
    // ~Simple();
    // Simple(const Simple&) = delete;
    // Simple& operator=(const Simple&) = delete;
    void update(const FrameInfo&) {}
    void render(const FrameInfo& frameInfo, Scene& scene);
  private:
    void createPipelineLayout(VkDescriptorSetLayout globalDescriptorSetLayout);
  };
}