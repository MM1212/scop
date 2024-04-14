#pragma once

#include <engine/renderer/Device.h>
#include <glm/glm.hpp>
#include <vector>

namespace Scop::Renderer {
  class Model {
  public:
    struct Vertex {
      glm::vec2 position;
      glm::vec3 color;

      static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
      static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
    };
    Model(Renderer::Device& device, const std::vector<Vertex>& vertices);
    ~Model();

    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);
  private:
    void createVertexBuffer(const std::vector<Vertex>& vertices);
  
    Renderer::Device& device;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    uint32_t vertexCount;
  };
}