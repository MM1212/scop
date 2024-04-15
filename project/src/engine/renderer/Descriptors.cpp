#include "engine/renderer/Descriptors.h"

using namespace Scop::Renderer;

DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::addBinding(
  uint32_t binding,
  VkDescriptorType descriptorType,
  VkShaderStageFlags stageFlags,
  uint32_t count
) {
  assert(bindings.count(binding) == 0 && "Binding already in use");
  VkDescriptorSetLayoutBinding layoutBinding{};
  layoutBinding.binding = binding;
  layoutBinding.descriptorType = descriptorType;
  layoutBinding.descriptorCount = count;
  layoutBinding.stageFlags = stageFlags;
  bindings[binding] = layoutBinding;
  return *this;
}

std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::build() const {
  return std::make_unique<DescriptorSetLayout>(this->device, this->bindings);
}

// 

DescriptorSetLayout::DescriptorSetLayout(Device& device, const BindingsMap& bindings)
  : device{ device }, bindings{ bindings } {
  std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
  for (auto kv : bindings) {
    setLayoutBindings.push_back(kv.second);
  }

  VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
  descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
  descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

  if (vkCreateDescriptorSetLayout(
    device.getHandle(),
    &descriptorSetLayoutInfo,
    nullptr,
    &this->handle) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }
}

DescriptorSetLayout::~DescriptorSetLayout() {
  vkDestroyDescriptorSetLayout(this->device.getHandle(), this->handle, nullptr);
}

// 

DescriptorPool::Builder& DescriptorPool::Builder::addPoolSize(
  VkDescriptorType descriptorType,
  uint32_t count
) {
  poolSizes.push_back({ descriptorType, count });
  return *this;
}

std::unique_ptr<DescriptorPool> DescriptorPool::Builder::build() const {
  return std::make_unique<DescriptorPool>(this->device, this->maxSets, this->poolFlags, this->poolSizes);
}

// 

DescriptorPool::DescriptorPool(
  Device& device,
  uint32_t maxSets,
  VkDescriptorPoolCreateFlags poolFlags,
  const std::vector<VkDescriptorPoolSize>& poolSizes
) : device{ device } {
  VkDescriptorPoolCreateInfo descriptorPoolInfo{};
  descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  descriptorPoolInfo.pPoolSizes = poolSizes.data();
  descriptorPoolInfo.maxSets = maxSets;
  descriptorPoolInfo.flags = poolFlags;

  if (vkCreateDescriptorPool(this->device.getHandle(), &descriptorPoolInfo, nullptr, &this->handle) !=
    VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool!");
  }
}

DescriptorPool::~DescriptorPool() {
  vkDestroyDescriptorPool(this->device.getHandle(), this->handle, nullptr);
}

bool DescriptorPool::allocSet(
  const VkDescriptorSetLayout descriptorSetLayout,
  VkDescriptorSet& descriptorSet
) const {
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = this->handle;
  allocInfo.pSetLayouts = &descriptorSetLayout;
  allocInfo.descriptorSetCount = 1;

  // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
  // a new pool whenever an old pool fills up. But this is beyond our current scope
  if (vkAllocateDescriptorSets(this->device.getHandle(), &allocInfo, &descriptorSet) != VK_SUCCESS) {
    return false;
  }
  return true;
}

void DescriptorPool::freeSets(const std::vector<VkDescriptorSet>& descriptorSets) const {
  vkFreeDescriptorSets(
    this->device.getHandle(),
    this->handle,
    static_cast<uint32_t>(descriptorSets.size()),
    descriptorSets.data()
  );
}

void DescriptorPool::reset() {
  vkResetDescriptorPool(this->device.getHandle(), this->handle, 0);
}

// 

DescriptorWriter::DescriptorWriter(DescriptorSetLayout& setLayout, DescriptorPool& pool)
  : setLayout{ setLayout }, pool{ pool } {}

DescriptorWriter& DescriptorWriter::write(uint32_t binding, std::function<void(VkWriteDescriptorSet&)> cb) {
  assert(this->setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

  auto& bindingDescription = this->setLayout.bindings[binding];

  assert(
    bindingDescription.descriptorCount == 1 &&
    "Binding single descriptor info, but binding expects multiple");

  VkWriteDescriptorSet write{};
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.descriptorType = bindingDescription.descriptorType;
  write.dstBinding = binding;
  write.descriptorCount = 1;
  cb(write); // Call the callback to set the rest of the fields

  writes.push_back(write);
  return *this;
}


DescriptorWriter& DescriptorWriter::write(uint32_t binding, VkDescriptorImageInfo* imageInfo) {
  return this->write(binding, [imageInfo](VkWriteDescriptorSet& write) { write.pImageInfo = imageInfo; });
}

DescriptorWriter& DescriptorWriter::write(uint32_t binding, VkDescriptorBufferInfo* bufferInfo) {
  return this->write(binding, [bufferInfo](VkWriteDescriptorSet& write) { write.pBufferInfo = bufferInfo; });
}

bool DescriptorWriter::build(VkDescriptorSet& set) {
  bool success = this->pool.allocSet(this->setLayout.getHandle(), set);
  if (!success) {
    return false;
  }
  this->overwrite(set);
  return true;
}

void DescriptorWriter::overwrite(VkDescriptorSet& set) {
  for (auto& write : this->writes) {
    write.dstSet = set;
  }
  vkUpdateDescriptorSets(this->pool.device.getHandle(), this->writes.size(), this->writes.data(), 0, nullptr);
}
