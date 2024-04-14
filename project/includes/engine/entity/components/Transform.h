#pragma once

#include <glm/glm.hpp>

namespace Scop::Components {
  struct Transform {
    glm::vec2 translation{};
    glm::vec2 scale{ 1.0f, 1.0f };
    float rotation{ 0.0f };

    Transform() = default;
    Transform(const Transform&) = default;
    Transform& operator=(const Transform&) = default;

    explicit operator glm::mat2() const {
      const float sin = glm::sin(this->rotation);
      const float cos = glm::cos(this->rotation);
      glm::mat2 rotationMatrix{
        {cos, sin},
        {-sin, cos}
      };
      glm::mat2 scaleMatrix{{this->scale.x, 0.0f}, {0.0f, this->scale.y}};
      return rotationMatrix * scaleMatrix;
    }
  };
}