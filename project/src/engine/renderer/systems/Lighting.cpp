#include "engine/renderer/systems/Lighting.h"

#include <glm/glm.hpp>
#include <iostream>

using Scop::Renderer::Systems::Lighting;

void Lighting::update(FrameInfo& frameInfo, Scene& scene) {
  auto rotateLight = glm::rotate(glm::mat4(1.f), 0.5f * frameInfo.deltaTime, { 0.f, -1.f, 0.f });
  GlobalUbo& ubo = frameInfo.globalUbo;
  auto view = scene.viewEntitiesWith<Components::PointLight, Components::Transform>();
  uint32_t lightIndex = 0;
  for (auto entity : view) {
    auto [pointLight, transform] = view.get<Components::PointLight, Components::Transform>(entity);
    if (lightIndex >= MAX_LIGHTS) break;
    // update light position
    transform.translation = glm::vec3(rotateLight * glm::vec4(transform.translation, 1.f));
    ubo.pointLights[lightIndex].position = glm::vec4(transform.translation, 1.0f);
    ubo.pointLights[lightIndex].color = pointLight.color;
    ubo.pointLights[lightIndex].range = pointLight.range;
    lightIndex++;
  }
  ubo.pointLightCount = lightIndex;
  {
    auto view = scene.viewEntitiesWith<Components::GlobalLight, Components::Transform>();
    if (view.size_hint() == 0) return;  // No global light in the scene
    else if (view.size_hint() > 1) throw std::runtime_error("Only one global light is allowed in the scene");
    auto [globalLight, transform] = view.get<Components::GlobalLight, Components::Transform>(*view.begin());
    ubo.ambientLight.position = glm::vec4(transform.translation, 1.0f);
    ubo.ambientLight.color = globalLight.color;
    ubo.ambientLight.range = -1.f;
  }
}