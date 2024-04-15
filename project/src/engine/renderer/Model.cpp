#include "engine/renderer/Model.h"
#include <cassert>
#include <cstring>
#include <stdexcept>

using Scop::Renderer::Model;

Model::Model(
  Renderer::Device& device,
  const std::vector<Vertex>& vertices
) : device{ device } {
  this->createVertexBuffer(vertices);
}

Model::~Model() {
  vkDestroyBuffer(this->device.getHandle(), this->vertexBuffer, nullptr);
  vkFreeMemory(this->device.getHandle(), this->vertexBufferMemory, nullptr);
}

void Model::createVertexBuffer(const std::vector<Vertex>& vertices) {
  this->vertexCount = vertices.size();
  assert(this->vertexCount >= 3 && "vertex count must be at least 3");
  VkDeviceSize bufferSize = sizeof(Vertex) * this->vertexCount;
  this->device.createBuffer(
    bufferSize,
    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    this->vertexBuffer,
    this->vertexBufferMemory
  );
  void* data;
  if (vkMapMemory(this->device.getHandle(), this->vertexBufferMemory, 0, bufferSize, 0, &data) != VK_SUCCESS)
    throw std::runtime_error("failed to map vertex buffer memory");
  std::memmove(data, vertices.data(), bufferSize);
  vkUnmapMemory(this->device.getHandle(), this->vertexBufferMemory);
}

void Model::bind(VkCommandBuffer commandBuffer) {
  VkBuffer buffers[] = { this->vertexBuffer };
  VkDeviceSize offsets[] = { 0 };
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
}

void Model::draw(VkCommandBuffer commandBuffer) {
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