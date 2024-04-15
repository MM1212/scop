#pragma once

#include <cstdint>
#include <engine/scene/SceneCamera.h>
#include <vulkan/vulkan.h>

namespace Scop::Renderer {
  struct FrameInfo {
    float deltaTime;
    uint32_t frameIndex;
    VkCommandBuffer commandBuffer;
    const SceneCamera& sceneCamera;
    VkDescriptorSet globalDescriptorSet;
  };
}