#pragma once

#include <string_view>
#include <vector>
#include <engine/renderer/Device.h>

namespace Scop::Renderer {
  class Pipeline {
  public:
    struct ConfigInfo {
      ConfigInfo() = default;
      ConfigInfo(const ConfigInfo&) = delete;
      ConfigInfo& operator=(const ConfigInfo&) = delete;

      std::vector<VkVertexInputBindingDescription> bindingDescriptions;
      std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
      VkPipelineViewportStateCreateInfo viewportInfo;
      VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
      VkPipelineRasterizationStateCreateInfo rasterizerInfo;
      VkPipelineMultisampleStateCreateInfo multisamplingInfo;
      VkPipelineColorBlendAttachmentState colorBlendAttachment;
      VkPipelineColorBlendStateCreateInfo colorBlendingInfo;
      VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
      std::vector<VkDynamicState> dynamicStateEnables;
      VkPipelineDynamicStateCreateInfo dynamicStateInfo;
      VkPipelineLayout pipelineLayout = nullptr;
      VkRenderPass renderPass = nullptr;
      uint32_t subpass = 0;
    };
    Pipeline(
      Device& device,
      const std::string_view vertFilePath,
      const std::string_view fragFilePath,
      const ConfigInfo& configInfo
    );
    ~Pipeline();
    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    void bind(VkCommandBuffer commandBuffer);
    static void SetupDefaultConfigInfo(ConfigInfo& configInfo);
  private:
    static std::vector<uint8_t> ReadFile(const std::string_view filePath);

    void createGraphicsPipeline(
      const std::string_view vertFilePath,
      const std::string_view fragFilePath,
      const ConfigInfo& configInfo
    );

    void createShaderModule(const std::vector<uint8_t>& code, VkShaderModule* shaderModule);

    Device& device;
    VkPipeline graphicsPipeline;
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
  };
}