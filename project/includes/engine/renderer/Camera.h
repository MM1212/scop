#pragma once

#include <glm/glm.hpp>

namespace Scop::Renderer {
  class Camera {
  public:
    Camera() = default;
    Camera(const glm::mat4& projection) : projection(projection) {}
    Camera(const Camera&) = default;
    Camera& operator=(const Camera&) = default;
    virtual ~Camera() = default;

    const glm::mat4& getProjection() const { return this->projection; }
  protected:
    glm::mat4 projection{ 1.0f };
  };
}