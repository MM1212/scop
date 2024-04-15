#include "engine/renderer/Model.h"
#include <cassert>
#include <cstring>
#include <stdexcept>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

using Scop::Renderer::Model;

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
  std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);

  // Vertex::position
  attributeDescriptions[0].binding = 0;
  attributeDescriptions[0].location = 0;
  attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  attributeDescriptions[0].offset = offsetof(Vertex, position);

  // Vertex::color
  attributeDescriptions[1].binding = 0;
  attributeDescriptions[1].location = 1;
  attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  attributeDescriptions[1].offset = offsetof(Vertex, color);

  return attributeDescriptions;
}