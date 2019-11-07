#pragma once
#include "RenderEntity.h"
#include "VulkanContext.h"
#include "VulkanAllocator.h"
#include "RenderEntityCreateInfo.h"
#include "ImageFactory.h"

class RenderEntityFactory {
public:
	RenderEntityFactory(VulkanContext* vulkanContext, VulkanAllocator* vulkanAllocator);
	~RenderEntityFactory();
	
	 RenderEntity* CreateRenderEntity(RenderEntityCreateInfo& createInfo, RenderTransform* transform, bool debugEntity);
	 void RecreateGraphicsPipeline(RenderEntity* renderEntity);
private:
	VulkanContext* renderDataVulkanContext;
	VulkanAllocator* vulkanAllocator;
	ImageFactory* imageFactory;

	 void CreateGraphicsPipeline(RenderDataUniform* renderDataSingular, std::string vert, std::string frag, VkPipeline& pipeline, VkPipelineLayout& pipelineLayout, RenderMode renderMode, bool instancing);
	 VkShaderModule CreateShaderModule(const std::vector<char>& code, VkDevice& device) const;
	static std::vector<VkVertexInputAttributeDescription> CreateVertexAttributeDescription(bool instancing);
	static std::vector<VkVertexInputBindingDescription> CreateVertexBindingDescription(bool instancing);
	 std::vector<char> ReadFile(const std::string& filename);	

	 void CreateInstanceBuffer(RenderDataInstanced* renderDataInstanced);

	 void CreateUniformBuffers(RenderDataUniform* renderDataSingular) const;

	 void CreateDescriptorSetLayout(RenderDataUniform* renderDataSingular) const;	
	 void CreateDescriptorPool(RenderDataUniform* renderDataSingular) const;
	 void CreateDescriptorSets(RenderDataUniform* renderDataSingular) const;
};

