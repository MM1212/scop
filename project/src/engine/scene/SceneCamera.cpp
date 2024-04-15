#include "engine/scene/SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

using Scop::SceneCamera;

SceneCamera::SceneCamera() {
  this->computeProjection();
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
}

void SceneCamera::setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
  this->viewMatrix = glm::lookAt(position, position + direction, up);
}
void SceneCamera::setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
  this->setViewDirection(position, target - position, up);
}
void SceneCamera::setViewYXZ(glm::vec3 position, glm::vec3 rotation) {
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
}