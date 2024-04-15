#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Scop::Components {
  struct Transform {
    glm::vec3 translation{};
    glm::vec3 scale{ 1.f, 1.f, 1.f };
    glm::vec3 rotation{};

    Transform() = default;
    Transform(const Transform&) = default;
    Transform& operator=(const Transform&) = default;

    // translate * R(Y) * R(X) * R(Z) * scale
    explicit operator glm::mat4() const {
      auto transform = glm::translate(glm::mat4{ 1.f }, translation);
      transform = glm::rotate(transform, rotation.y, glm::vec3{ 0.f, 1.f, 0.f });
      transform = glm::rotate(transform, rotation.x, glm::vec3{ 1.f, 0.f, 0.f });
      transform = glm::rotate(transform, rotation.z, glm::vec3{ 0.f, 0.f, 1.f });
      transform = glm::scale(transform, scale);
      return transform;
    }
  };
}