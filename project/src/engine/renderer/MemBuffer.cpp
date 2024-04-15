#include "engine/renderer/MemBuffer.h"

#include <cstring>
#include <cassert>

using Scop::Renderer::MemBuffer;

/**
 * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
 *
 * @param instanceSize The size of an instance
 * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
 * minUniformBufferOffsetAlignment)
 *
 * @return VkResult of the buffer mapping call
 */
VkDeviceSize MemBuffer::GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
  if (minOffsetAlignment > 0) {
    return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
  }
  return instanceSize;
}

MemBuffer::MemBuffer(
  Device& device,
  VkDeviceSize instanceSize,
  uint32_t instanceCount,
  VkBufferUsageFlags usageFlags,
  VkMemoryPropertyFlags memoryPropertyFlags,
  VkDeviceSize minOffsetAlignment)
  :
  device{ device }, instanceCount{ instanceCount }, instanceSize{ instanceSize },
  usageFlags{ usageFlags }, memoryPropertyFlags{ memoryPropertyFlags } {
  this->alignmentSize = GetAlignment(instanceSize, minOffsetAlignment);
  this->size = alignmentSize * instanceCount;
  device.createBuffer(this->size, usageFlags, memoryPropertyFlags, buffer, memory);
}

MemBuffer::~MemBuffer() {
  unmap();
  vkDestroyBuffer(device.getHandle(), buffer, nullptr);
  vkFreeMemory(device.getHandle(), memory, nullptr);
}

/**
 * Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
 *
 * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
 * buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the buffer mapping call
 */
VkResult MemBuffer::map(VkDeviceSize size, VkDeviceSize offset) {
  assert(buffer && memory && "Called map on buffer before create");
  return vkMapMemory(device.getHandle(), memory, offset, size, 0, &mapped);
}

/**
 * Unmap a mapped memory range
 *
 * @note Does not return a result as vkUnmapMemory can't fail
 */
void MemBuffer::unmap() {
  if (mapped) {
    vkUnmapMemory(device.getHandle(), memory);
    mapped = nullptr;
  }
}

/**
 * Copies the specified data to the mapped buffer. Default value writes whole buffer range
 *
 * @param data Pointer to the data to copy
 * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
 * range.
 * @param offset (Optional) Byte offset from beginning of mapped region
 *
 */
void MemBuffer::writeTo(const void* data, VkDeviceSize size, VkDeviceSize offset) {
  assert(mapped && "Cannot copy to unmapped buffer");

  if (size == VK_WHOLE_SIZE) {
    std::memmove(mapped, data, this->size);
  }
  else {
    char* memOffset = reinterpret_cast<char*>(mapped);
    memOffset += offset;
    std::memmove(memOffset, data, size);
  }
}

/**
 * Flush a memory range of the buffer to make it visible to the device
 *
 * @note Only required for non-coherent memory
 *
 * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the
 * complete buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the flush call
 */
VkResult MemBuffer::flush(VkDeviceSize size, VkDeviceSize offset) {
  VkMappedMemoryRange mappedRange = {};
  mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  mappedRange.memory = memory;
  mappedRange.offset = offset;
  mappedRange.size = size;
  return vkFlushMappedMemoryRanges(device.getHandle(), 1, &mappedRange);
}

/**
 * Invalidate a memory range of the buffer to make it visible to the host
 *
 * @note Only required for non-coherent memory
 *
 * @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
 * the complete buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the invalidate call
 */
VkResult MemBuffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
  VkMappedMemoryRange mappedRange = {};
  mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  mappedRange.memory = memory;
  mappedRange.offset = offset;
  mappedRange.size = size;
  return vkInvalidateMappedMemoryRanges(device.getHandle(), 1, &mappedRange);
}

/**
 * Create a buffer info descriptor
 *
 * @param size (Optional) Size of the memory range of the descriptor
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkDescriptorBufferInfo of specified offset and range
 */
VkDescriptorBufferInfo MemBuffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset) const {
  return VkDescriptorBufferInfo{
      buffer,
      offset,
      size,
  };
}

/**
 * Copies "instanceSize" bytes of data to the mapped buffer at an offset of index * alignmentSize
 *
 * @param data Pointer to the data to copy
 * @param index Used in offset calculation
 *
 */
void MemBuffer::writeToIndex(const void* data, uint32_t index) {
  writeTo(data, instanceSize, index * alignmentSize);
}

/**
 *  Flush the memory range at index * alignmentSize of the buffer to make it visible to the device
 *
 * @param index Used in offset calculation
 *
 */
VkResult MemBuffer::flushIndex(uint32_t index) { return flush(alignmentSize, index * alignmentSize); }

/**
 * Create a buffer info descriptor
 *
 * @param index Specifies the region given by index * alignmentSize
 *
 * @return VkDescriptorBufferInfo for instance at index
 */
VkDescriptorBufferInfo MemBuffer::descriptorInfoForIndex(uint32_t index) const {
  return descriptorInfo(alignmentSize, index * alignmentSize);
}

/**
 * Invalidate a memory range of the buffer to make it visible to the host
 *
 * @note Only required for non-coherent memory
 *
 * @param index Specifies the region to invalidate: index * alignmentSize
 *
 * @return VkResult of the invalidate call
 */
VkResult MemBuffer::invalidateIndex(uint32_t index) {
  return invalidate(alignmentSize, index * alignmentSize);
}