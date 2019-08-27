#pragma once
#include "RenderEntity.h"
#include "RenderDataVulkanContext.h"

class RenderEntityFactory {
public:
	RenderEntityFactory();
	~RenderEntityFactory();

	RenderEntity* CreateRenderEntity(RenderDataVulkanContext* renderDataVulkanBackend, RenderTransform* transform, bool isStatic, std::string vertexShader, std::string fragmentShader);

private:
	std::vector<char> ReadFile(const std::string& filename);
	VkShaderModule CreateShaderModule(const std::vector<char>& code, VkDevice& device);
	std::vector<VkVertexInputBindingDescription> CreateVertexBindingDescription(bool instancing);
	std::vector<VkVertexInputAttributeDescription> CreateVertexAttributeDescription(bool instancing);
	void CreateGraphicsPipeline(RenderDataVulkanContext& renderDataVulkanBackend, RenderDataSingular* renderDataSingular, std::string vert, std::string frag, VkPipeline& pipeline, VkPipelineLayout& pipelineLayout, bool instancing);
	void CreateInstanceBuffer(RenderDataVulkanContext& renderDataVulkanContext, RenderDataInstanced* renderDataInstanced);
	uint32_t FindMemoryType(RenderDataVulkanContext& vulkanContext, uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void CopyBuffer(RenderDataVulkanContext& vulkanContext, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void CreateDescriptorSetLayout(RenderDataVulkanContext& renderDataVulkanContext, RenderDataSingular* renderDataSingular);
	void CreateUniformBuffers(RenderDataVulkanContext& renderDataVulkanContext, RenderDataSingular* renderDataSingular);
	void CreateDescriptorPool(RenderDataVulkanContext& renderDataVulkanContext, RenderDataSingular* renderDataSingular);
	void CreateDescriptorSets(RenderDataVulkanContext& renderDataVulkanContext, RenderDataSingular* renderDataSingular);
	void CreateBuffer(RenderDataVulkanContext& vulkanContext, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
};

