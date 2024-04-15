#include "engine/scene/SceneCamera.h"
#include <engine/input/Input.h>

#include <glm/gtc/matrix_transform.hpp>
#include <limits>
#include <iostream>

using Scop::SceneCamera;

SceneCamera::SceneCamera() {
  this->computeProjection();
  this->setViewYXZ(this->transform.translation, this->transform.rotation);
}

void SceneCamera::setPerspective(float fov, float nearClip, float farClip) {
  this->projectionType = ProjectionType::Perspective;
  this->perspectiveFov = fov;
  this->perspectiveNearClip = nearClip;
  this->perspectiveFarClip = farClip;
  this->computeProjection();
}
void SceneCamera::setOrthographic(float size, float nearClip, float farClip) {
  this->projectionType = ProjectionType::Orthographic;
  this->orthographicSize = size;
  this->orthographicNearClip = nearClip;
  this->orthographicFarClip = farClip;
  this->computeProjection();
}
void SceneCamera::setViewportSize(uint32_t width, uint32_t height) {
  if (this->viewportSize.x == width && this->viewportSize.y == height)
    return;
  this->viewportSize = { width, height };
  this->aspectRatio = static_cast<float>(width) / static_cast<float>(height);
  this->computeProjection();
}
void SceneCamera::computeProjection() {
  switch (this->projectionType) {
  case ProjectionType::Perspective:
    this->projection = glm::perspective(this->perspectiveFov, this->aspectRatio, this->perspectiveNearClip, this->perspectiveFarClip);
    break;
  case ProjectionType::Orthographic:
    float orthoLeft = -this->orthographicSize * this->aspectRatio * 0.5f;
    float orthoRight = this->orthographicSize * this->aspectRatio * 0.5f;
    float orthoBottom = -this->orthographicSize * 0.5f;
    float orthoTop = this->orthographicSize * 0.5f;
    this->projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, this->orthographicNearClip, this->orthographicFarClip);
    break;
  };
  this->projectionViewMatrix = this->projection * this->viewMatrix;
}

void SceneCamera::setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
  this->viewMatrix = glm::lookAt(position, position + direction, up);
  this->transform.translation = position;
  this->transform.rotation = glm::vec3{ glm::asin(-this->viewMatrix[2][1]), glm::atan(this->viewMatrix[1][0], this->viewMatrix[0][0]), 0.f };
  this->projectionViewMatrix = this->projection * this->viewMatrix;
}
void SceneCamera::setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
  this->setViewDirection(position, target - position, up);
}
void SceneCamera::setViewYXZ(glm::vec3 position, glm::vec3 rotation) {
  this->transform.translation = position;
  this->transform.rotation = rotation;
  const float c3 = glm::cos(rotation.z);
  const float s3 = glm::sin(rotation.z);
  const float c2 = glm::cos(rotation.x);
  const float s2 = glm::sin(rotation.x);
  const float c1 = glm::cos(rotation.y);
  const float s1 = glm::sin(rotation.y);
  const glm::vec3 u{ (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
  const glm::vec3 v{ (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };
  const glm::vec3 w{ (c2 * s1), (-s2), (c1 * c2) };
  viewMatrix = glm::mat4{ 1.f };
  viewMatrix[0][0] = u.x;
  viewMatrix[1][0] = u.y;
  viewMatrix[2][0] = u.z;
  viewMatrix[0][1] = v.x;
  viewMatrix[1][1] = v.y;
  viewMatrix[2][1] = v.z;
  viewMatrix[0][2] = w.x;
  viewMatrix[1][2] = w.y;
  viewMatrix[2][2] = w.z;
  viewMatrix[3][0] = -glm::dot(u, position);
  viewMatrix[3][1] = -glm::dot(v, position);
  viewMatrix[3][2] = -glm::dot(w, position);
  this->projectionViewMatrix = this->projection * this->viewMatrix;
}

void SceneCamera::update(float deltaTime) {
  float lookSpeed = this->lookSpeed;
  float moveSpeed = this->moveSpeed;
  bool changed = false;
  glm::vec3 rotate{ 0 };

  if (Input::IsKeyPressed(Input::Key::Right))
    rotate.y -= 1.f;
  if (Input::IsKeyPressed(Input::Key::Left))
    rotate.y += 1.f;
  if (Input::IsKeyPressed(Input::Key::Up))
    rotate.x -= 1.f;
  if (Input::IsKeyPressed(Input::Key::Down))
    rotate.x += 1.f;

  if (Input::IsKeyPressed(Input::Key::LeftShift)) {
    moveSpeed *= 2.f;
  }
  else if (Input::IsKeyPressed(Input::Key::LeftControl)) {
    moveSpeed *= 0.5f;
  }
  if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
    this->transform.rotation += glm::normalize(rotate) * (lookSpeed * deltaTime);
    changed = true;
  }

  this->transform.rotation.x = glm::clamp(this->transform.rotation.x, -glm::half_pi<float>(), glm::half_pi<float>());
  this->transform.rotation.y = glm::mod(this->transform.rotation.y, glm::two_pi<float>());

  float yaw = this->transform.rotation.y;
  float pitch = this->transform.rotation.x;
  const glm::vec3 forwardDir = glm::normalize(glm::vec3{ glm::cos(pitch) * glm::sin(yaw), -glm::sin(pitch), glm::cos(pitch) * glm::cos(yaw) });
  const glm::vec3 rightDir = glm::normalize(glm::cross(forwardDir, glm::vec3{ 0.f, -1.f, 0.f }));
  constexpr glm::vec3 upDir{ 0.f, -1.f, 0.f };

  glm::vec3 move{ 0 };
  if (Input::IsKeyPressed(Input::Key::W))
    move -= forwardDir;
  if (Input::IsKeyPressed(Input::Key::S))
    move += forwardDir;
  if (Input::IsKeyPressed(Input::Key::D))
    move += rightDir;
  if (Input::IsKeyPressed(Input::Key::A))
    move -= rightDir;
  if (Input::IsKeyPressed(Input::Key::Q))
    move -= upDir;
  if (Input::IsKeyPressed(Input::Key::E))
    move += upDir;

  if (glm::dot(move, move) > std::numeric_limits<float>::epsilon()) {
    this->transform.translation += glm::normalize(move) * (moveSpeed * deltaTime);
    changed = true;
  }
  if (changed) {
    std::cout << "forward dir: " << forwardDir.x << " " << forwardDir.y << " " << forwardDir.z << std::endl;
    std::cout << "right dir: " << rightDir.x << " " << rightDir.y << " " << rightDir.z << std::endl;
    // std::cout << "Camera position: " << this->transform.translation.x << " " << this->transform.translation.y << " " << this->transform.translation.z << std::endl;
    // std::cout << "Camera rotation: " << this->transform.rotation.x << " " << this->transform.rotation.y << " " << this->transform.rotation.z << std::endl;
    this->setViewYXZ(this->transform.translation, this->transform.rotation);
  }
}