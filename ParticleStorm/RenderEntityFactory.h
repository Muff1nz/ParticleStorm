#pragma once
#include "RenderEntity.h"
#include "RenderDataVulkanContext.h"
#include "RenderEngineVulkanBackend.h"

class RenderEntityFactory {
public:
	RenderEntityFactory();
	~RenderEntityFactory();

	static RenderEntity* CreateRenderEntity(RenderEngineVulkanBackend* vulkanBackend, RenderDataVulkanContext* renderDataVulkanContext, RenderTransform* transform, const std::string& vertexShader, const std::string& fragmentShader);
private:
	static void CreateGraphicsPipeline(RenderDataVulkanContext& renderDataVulkanContext, RenderDataSingular* renderDataSingular, std::string vert, std::string frag, VkPipeline& pipeline, VkPipelineLayout& pipelineLayout, bool instancing);
	static VkShaderModule CreateShaderModule(const std::vector<char>& code, VkDevice& device);
	static std::vector<VkVertexInputAttributeDescription> CreateVertexAttributeDescription(bool instancing);
	static std::vector<VkVertexInputBindingDescription> CreateVertexBindingDescription(bool instancing);
	static std::vector<char> ReadFile(const std::string& filename);	

	static void CreateInstanceBuffer(RenderEngineVulkanBackend* vulkanBackend, RenderDataVulkanContext& renderDataVulkanContext, RenderDataInstanced* renderDataInstanced);

	static void CreateDescriptorSetLayout(RenderDataVulkanContext& renderDataVulkanContext, RenderDataSingular* renderDataSingular);
	static void CreateUniformBuffers(RenderEngineVulkanBackend* vulkanBackend, RenderDataVulkanContext& renderDataVulkanContext, RenderDataSingular* renderDataSingular);	
	static void CreateDescriptorPool(RenderDataVulkanContext& renderDataVulkanContext, RenderDataSingular* renderDataSingular);
	static void CreateDescriptorSets(RenderDataVulkanContext& renderDataVulkanContext, RenderDataSingular* renderDataSingular);
};

