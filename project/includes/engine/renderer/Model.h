#pragma once

#include <engine/renderer/Device.h>
#include <engine/renderer/MemBuffer.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace Scop::Renderer {
  class Model {
  public:
    struct Vertex {
      glm::vec3 position{};
      glm::vec3 color{};
      glm::vec3 normal{};
      glm::vec2 uv{};

      static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
      static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
    };
    struct Builder {
      std::vector<Vertex> vertices{};
      std::vector<uint32_t> indices{};
    };
    Model(Device& device, const Builder& builder);
    ~Model();

    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);
  private:
    void createVertexBuffer(const std::vector<Vertex>& vertices);
    void createIndexBuffer(const std::vector<uint32_t>& indices);
  
    Device& device;


    std::unique_ptr<MemBuffer> vertexBuffer;
    uint32_t vertexCount;

    bool hasIndexBuffer = false;
    std::unique_ptr<MemBuffer> indexBuffer;
    uint32_t indexCount;
  };
}