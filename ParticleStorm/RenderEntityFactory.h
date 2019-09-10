#pragma once
#include "RenderEntity.h"
#include "RenderDataVulkanContext.h"
#include "VulkanAllocator.h"
#include "RenderEntityCreateInfo.h"

class RenderEntityFactory {
public:
	RenderEntityFactory();
	~RenderEntityFactory();

	static RenderEntity* CreateRenderEntity(RenderEntityCreateInfo& createInfo, RenderDataVulkanContext* renderDataVulkanContext, VulkanAllocator* vulkanAllocator, RenderTransform
	                                        * transform, bool debugEntity);
private:
	static void CreateGraphicsPipeline(RenderDataVulkanContext& renderDataVulkanContext, RenderDataSingular* renderDataSingular, std::string vert, std::string frag, VkPipeline& pipeline, VkPipelineLayout& pipelineLayout, RenderMode renderMode, bool instancing);
	static VkShaderModule CreateShaderModule(const std::vector<char>& code, VkDevice& device);
	static std::vector<VkVertexInputAttributeDescription> CreateVertexAttributeDescription(bool instancing);
	static std::vector<VkVertexInputBindingDescription> CreateVertexBindingDescription(bool instancing);
	static std::vector<char> ReadFile(const std::string& filename);	

	static void CreateInstanceBuffer(VulkanAllocator* vulkanAllocator, RenderDataVulkanContext& renderDataVulkanContext, RenderDataInstanced* renderDataInstanced);

	static void CreateDescriptorSetLayout(RenderDataVulkanContext& renderDataVulkanContext, RenderDataSingular* renderDataSingular);
	static void CreateUniformBuffers(VulkanAllocator* vulkanAllocator, RenderDataVulkanContext& renderDataVulkanContext, RenderDataSingular* renderDataSingular);	
	static void CreateDescriptorPool(RenderDataVulkanContext& renderDataVulkanContext, RenderDataSingular* renderDataSingular);
	static void CreateDescriptorSets(RenderDataVulkanContext& renderDataVulkanContext, RenderDataSingular* renderDataSingular);
};

