#pragma once

#include <cstdint>
#include <engine/scene/SceneCamera.h>
#include <engine/scene/components/Lights.h>
#include <vulkan/vulkan.h>

namespace Scop::Renderer {
#define MAX_LIGHTS 16

  template <typename T>
  struct Light : public T {
    glm::vec4 position;
  };

  struct GlobalUbo {
    glm::mat4 projection{ 05.f };
    glm::mat4 view{ 1.f };
    glm::mat4 projectionView{ 1.f };
    glm::mat4 inverseView{ 1.f };
    Light<Components::GlobalLight> ambientLight{
      Components::GlobalLight{ 0.01f, glm::vec3{ 1.0f } },
      glm::vec4{-1.f},
    };
    Light<Components::PointLight> pointLights[MAX_LIGHTS]{};
    uint32_t pointLightCount = 0;
  };
  struct FrameInfo {
    float deltaTime;
    uint32_t frameIndex;
    VkCommandBuffer commandBuffer;
    const SceneCamera& sceneCamera;
    VkDescriptorSet globalDescriptorSet;
    GlobalUbo globalUbo;
  };
}