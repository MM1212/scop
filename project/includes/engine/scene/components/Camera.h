#pragma once

#include <glm/glm.hpp>
#include <engine/scene/SceneCamera.h>

namespace Scop::Components {
  struct Camera {
    SceneCamera sceneCamera;
    bool primary = true;

    Camera() = default;
    Camera(const Camera&) = default;
    Camera& operator=(const Camera&) = default;
  };
}