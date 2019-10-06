#pragma once
#include "RenderEntity.h"
#include "RenderDataVulkanContext.h"
#include "VulkanAllocator.h"
#include "RenderEntityCreateInfo.h"

class RenderEntityFactory {
public:
	RenderEntityFactory(RenderDataVulkanContext* renderDataVulkanContext, VulkanAllocator* vulkanAllocator);
	~RenderEntityFactory();
	
	 RenderEntity* CreateRenderEntity(RenderEntityCreateInfo& createInfo, RenderTransform* transform, bool debugEntity);
	 void RecreateGraphicsPipeline(RenderEntity* renderEntity);
private:
	RenderDataVulkanContext* renderDataVulkanContext;
	VulkanAllocator* vulkanAllocator;

	 void CreateGraphicsPipeline(RenderDataSingular* renderDataSingular, std::string vert, std::string frag, VkPipeline& pipeline, VkPipelineLayout& pipelineLayout, RenderMode renderMode, bool instancing);
	 VkShaderModule CreateShaderModule(const std::vector<char>& code, VkDevice& device) const;
	static std::vector<VkVertexInputAttributeDescription> CreateVertexAttributeDescription(bool instancing);
	static std::vector<VkVertexInputBindingDescription> CreateVertexBindingDescription(bool instancing);
	 std::vector<char> ReadFile(const std::string& filename);	

	 void CreateInstanceBuffer(RenderDataInstanced* renderDataInstanced);

	 void CreateUniformBuffers(RenderDataSingular* renderDataSingular);

	void CreateTextureImage(std::string texturePath, RenderDataSingular* renderDataSingular);
	void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void CreateTextureImageView(RenderDataSingular* renderDataSingular) const;
	void CreateTextureSampler(RenderDataSingular* renderDataSingular) const;

	VkCommandBuffer BeginSingleTimeCommands() const;
	void EndSingleTimeCommands(VkCommandBuffer commandBuffer) const;

	 void CreateDescriptorSetLayout(RenderDataSingular* renderDataSingular);	
	 void CreateDescriptorPool(RenderDataSingular* renderDataSingular) const;
	 void CreateDescriptorSets(RenderDataSingular* renderDataSingular) const;
};

