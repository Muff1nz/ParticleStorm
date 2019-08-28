#pragma once
#include "RenderEntity.h"
#include "RenderDataVulkanContext.h"

class RenderEntityFactory {
public:
	RenderEntityFactory();
	~RenderEntityFactory();

	static RenderEntity* CreateRenderEntity(RenderDataVulkanContext* renderDataVulkanContext, RenderTransform* transform, const std::string& vertexShader, const std::string& fragmentShader);

private:
	static std::vector<char> ReadFile(const std::string& filename);
	static VkShaderModule CreateShaderModule(const std::vector<char>& code, VkDevice& device);
	static std::vector<VkVertexInputBindingDescription> CreateVertexBindingDescription(bool instancing);
	static std::vector<VkVertexInputAttributeDescription> CreateVertexAttributeDescription(bool instancing);
	static void CreateGraphicsPipeline(RenderDataVulkanContext& renderDataVulkanContext, RenderDataSingular* renderDataSingular, std::string vert, std::string frag, VkPipeline& pipeline, VkPipelineLayout& pipelineLayout, bool instancing);
	static void CreateInstanceBuffer(RenderDataVulkanContext& renderDataVulkanContext, RenderDataInstanced* renderDataInstanced);
	static uint32_t FindMemoryType(RenderDataVulkanContext& renderDataVulkanContext, uint32_t typeFilter, VkMemoryPropertyFlags properties);
	static void CopyBuffer(RenderDataVulkanContext& renderDataVulkanContext, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	static void CreateDescriptorSetLayout(RenderDataVulkanContext& renderDataVulkanContext, RenderDataSingular* renderDataSingular);
	static void CreateUniformBuffers(RenderDataVulkanContext& renderDataVulkanContext, RenderDataSingular* renderDataSingular);
	static void CreateDescriptorPool(RenderDataVulkanContext& renderDataVulkanContext, RenderDataSingular* renderDataSingular);
	static void CreateDescriptorSets(RenderDataVulkanContext& renderDataVulkanContext, RenderDataSingular* renderDataSingular);
	static void CreateBuffer(RenderDataVulkanContext& renderDataVulkanContext, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
};

