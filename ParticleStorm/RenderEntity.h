#pragma once
#include "RenderDataCore.h"
#include "RenderDataUniform.h"
#include "RenderDataInstanced.h"
#include "VulkanContext.h"
#include "Environment.h"
#include "RenderEntityMeta.h"

class RenderEntity {
public:
	friend class RenderEntityFactory;

	RenderEntity(VulkanContext* renderDataVulkanContext, RenderDataCore* renderDataCore, RenderDataUniform* renderDataSingular, RenderDataInstanced* renderDataInstanced, RenderEntityMeta* renderEntityMeta, bool debugEntity);
	~RenderEntity();

	void Dispose();

	void UpdateBuffers(uint32_t imageIndex, Camera* camera) const;
	void BindToCommandPool(std::vector<VkCommandBuffer>& commandBuffers, int index) const;
	bool IsDebugEntity() const;

private:
	bool isDisposed = false;
	bool debugEntity = false;

	VulkanContext* renderDataVulkanContext;

	RenderEntityMeta* renderEntityMeta;
	RenderDataCore* renderDataCore;
	RenderDataUniform* renderDataUniform;
	RenderDataInstanced* renderDataInstanced;

	void UpdateInstanceBuffer(uint32_t imageIndex, Camera* camera) const;
	void UpdateUniformBuffer(uint32_t imageIndex, Camera* camera) const;
	void DisposePipeline() const;
};
