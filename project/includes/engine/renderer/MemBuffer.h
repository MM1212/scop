#pragma once

#include <engine/renderer/Device.h>

namespace Scop::Renderer {
  class MemBuffer {
  public:
    MemBuffer(
      Device& device,
      VkDeviceSize instanceSize,
      uint32_t instanceCount,
      VkBufferUsageFlags usageFlags,
      VkMemoryPropertyFlags memoryPropertyFlags,
      VkDeviceSize minOffsetAlignment = 1
    );
    ~MemBuffer();

    MemBuffer(const MemBuffer&) = delete;
    MemBuffer& operator=(const MemBuffer&) = delete;

    VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    void unmap();

    void writeTo(const void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkDescriptorBufferInfo getDescriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) const;
    VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

    void writeToIndex(const void* data, uint32_t index);
    VkResult flushIndex(uint32_t index);
    VkDescriptorBufferInfo descriptorInfoForIndex(uint32_t index) const;
    VkResult invalidateIndex(uint32_t index);

    explicit operator VkBuffer() const { return this->buffer; }
    explicit operator VkDeviceMemory() const { return this->memory; }

    VkBuffer getHandle() const { return this->buffer; }
    void* getMappedMemory() const { return this->mapped; }
    uint32_t getInstanceCount() const { return this->instanceCount; }
    VkDeviceSize getInstanceSize() const { return this->instanceSize; }
    VkDeviceSize getAlignmentSize() const { return this->instanceSize; }
    VkBufferUsageFlags getUsageFlags() const { return this->usageFlags; }
    VkMemoryPropertyFlags getMemoryPropertyFlags() const { return this->memoryPropertyFlags; }
    VkDeviceSize getSize() const { return this->size; }
  private:
    static VkDeviceSize GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

    Device& device;
    void* mapped = nullptr;
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;

    VkDeviceSize size = 0;
    uint32_t instanceCount;
    VkDeviceSize instanceSize;
    VkDeviceSize alignmentSize;
    VkBufferUsageFlags usageFlags;
    VkMemoryPropertyFlags memoryPropertyFlags;
  };
}