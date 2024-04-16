#pragma once

#include "Base.h"

namespace Scop::Renderer::Systems {
  class Billboards : public Base {
  public:
    Billboards(const SystemInfo& deps);

    void update(const FrameInfo&) {}
    void render(const FrameInfo& frameInfo, Scene& scene);
  private:
    void createPipelineLayout(VkDescriptorSetLayout globalDescriptorSetLayout);
  };
}