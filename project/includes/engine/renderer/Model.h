#pragma once

#include <engine/renderer/Device.h>
#include <engine/renderer/MemBuffer.h>
#include <glm/glm.hpp>

#include <vector>
#include <memory>
#include <string_view>

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

      bool operator==(const Vertex& other) const {
        return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
      }
    };
    struct Builder {
      std::vector<Vertex> vertices{};
      std::vector<uint32_t> indices{};

      bool loadModel(const std::string_view filePath);
    };
    Model(Device& device, const Builder& builder);
    ~Model();

    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    static std::unique_ptr<Model> CreateFromFile(Device& device, const std::string_view filePath);

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