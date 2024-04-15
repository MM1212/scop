#include "engine/renderer/Model.h"
#include <utils/hash.h>

#include <cassert>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

using Scop::Renderer::Model;
using Scop::Utils::HashCombine;

namespace std {
  template<> struct hash<Model::Vertex> {
    size_t operator()(const Model::Vertex& vertex) const {
      size_t seed = 0;
      HashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
      return seed;
    }
  };
};

Model::Model(
  Renderer::Device& device,
  const Builder& builder
) : device{ device } {
  this->createVertexBuffer(builder.vertices);
  this->createIndexBuffer(builder.indices);
}

Model::~Model() {}

void Model::createVertexBuffer(const std::vector<Vertex>& vertices) {
  this->vertexCount = vertices.size();
  assert(this->vertexCount >= 3 && "vertex count must be at least 3");
  uint32_t vertexSize = sizeof(Vertex);
  VkDeviceSize bufferSize = vertexSize * this->vertexCount;
  MemBuffer stagingBuffer{
    this->device,
    vertexSize,
    vertexCount,
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  };

  stagingBuffer.map();
  stagingBuffer.writeTo(vertices.data());

  this->vertexBuffer = std::make_unique<MemBuffer>(
    this->device,
    vertexSize,
    vertexCount,
    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
  );

  this->device.copyBuffer(
    static_cast<VkBuffer>(stagingBuffer),
    static_cast<VkBuffer>(*this->vertexBuffer),
    bufferSize
  );
}

void Model::createIndexBuffer(const std::vector<uint32_t>& indices) {
  this->indexCount = indices.size();
  this->hasIndexBuffer = this->indexCount > 0;
  if (!this->hasIndexBuffer)
    return;
  uint32_t indexSize = sizeof(uint32_t);
  VkDeviceSize bufferSize = indexSize * this->indexCount;

  MemBuffer stagingBuffer{
    this->device,
    indexSize,
    this->indexCount,
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  };

  stagingBuffer.map();
  stagingBuffer.writeTo(indices.data());

  this->indexBuffer = std::make_unique<MemBuffer>(
    this->device,
    indexSize,
    this->indexCount,
    VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
  );

  this->device.copyBuffer(
    static_cast<VkBuffer>(stagingBuffer),
    static_cast<VkBuffer>(*this->indexBuffer),
    bufferSize
  );
}

void Model::bind(VkCommandBuffer commandBuffer) {
  VkBuffer buffers[] = { static_cast<VkBuffer>(*this->vertexBuffer) };
  VkDeviceSize offsets[] = { 0 };
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
  if (this->hasIndexBuffer)
    vkCmdBindIndexBuffer(
      commandBuffer,
      static_cast<VkBuffer>(*this->indexBuffer),
      0, VK_INDEX_TYPE_UINT32
    );
}

void Model::draw(VkCommandBuffer commandBuffer) {
  if (this->hasIndexBuffer)
    vkCmdDrawIndexed(commandBuffer, this->indexCount, 1, 0, 0, 0);
  else
    vkCmdDraw(commandBuffer, this->vertexCount, 1, 0, 0);
}

std::vector<VkVertexInputBindingDescription> Model::Vertex::GetBindingDescriptions() {
  std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
  bindingDescriptions[0].binding = 0;
  bindingDescriptions[0].stride = sizeof(Vertex);
  bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> Model::Vertex::GetAttributeDescriptions() {
  std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

  attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
  attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
  attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
  attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });

  return attributeDescriptions;
}

std::unique_ptr<Model> Model::CreateFromFile(Device& device, const std::string_view filePath) {
  Builder builder{};
  if (!builder.loadModel(filePath))
    return nullptr;
  std::cout << "Loaded model " << filePath << " with " << builder.vertices.size() << " vertices" << std::endl;
  return std::make_unique<Model>(device, builder);
}

bool Model::Builder::loadModel(const std::string_view filePath) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.data())) {
    std::cerr << "Failed to load model file " << filePath << std::endl;
    std::cerr << warn + err << std::endl;
    return false;
  }

  this->vertices.clear();
  this->indices.clear();

  std::unordered_map<Vertex, uint32_t> uniqueVertices{};
  for (const auto& shape : shapes) {
    for (const auto& index : shape.mesh.indices) {
      Vertex vertex{};
      if (index.vertex_index >= 0) {
        vertex.position = {
          attrib.vertices[3 * index.vertex_index + 0],
          attrib.vertices[3 * index.vertex_index + 1],
          attrib.vertices[3 * index.vertex_index + 2]
        };

        vertex.color = {
          attrib.colors[3 * index.vertex_index + 0],
          attrib.colors[3 * index.vertex_index + 1],
          attrib.colors[3 * index.vertex_index + 2]
        };
      }
      if (index.normal_index >= 0) {
        vertex.normal = {
          attrib.normals[3 * index.normal_index + 0],
          attrib.normals[3 * index.normal_index + 1],
          attrib.normals[3 * index.normal_index + 2]
        };
      }
      if (index.texcoord_index >= 0) {
        vertex.uv = {
          attrib.texcoords[2 * index.texcoord_index + 0],
          attrib.texcoords[2 * index.texcoord_index + 1]
        };
      }
      if (uniqueVertices.count(vertex) == 0) {
        uniqueVertices[vertex] = static_cast<uint32_t>(this->vertices.size());
        this->vertices.push_back(vertex);
      }
      this->indices.push_back(uniqueVertices[vertex]);
    }
  }
  return true;
}