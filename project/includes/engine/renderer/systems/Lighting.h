#pragma once

#include "Base.h"

namespace Scop::Renderer::Systems {
  class Lighting {
  public:
    Lighting() = default;

    void update(FrameInfo& frameInfo, Scene& scene);
    void render(const FrameInfo&, Scene&) {}
  };
}