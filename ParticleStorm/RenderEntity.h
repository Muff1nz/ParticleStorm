#pragma once
#include "RenderDataCore.h"
#include "RenderDataSingular.h"
#include "RenderDataInstanced.h"
#include "RenderDataVulkanContext.h"
#include "Environment.h"
#include "RenderEntityMeta.h"

class RenderEntity {
public:
	friend class RenderEntityFactory;

	RenderEntity(RenderDataVulkanContext* renderDataVulkanContext, RenderDataCore* renderDataCore, RenderDataSingular* renderDataSingular, RenderDataInstanced* renderDataInstanced, RenderEntityMeta* renderEntityMeta, bool debugEntity);
	~RenderEntity();

	void Dispose();

	void UpdateBuffers(uint32_t imageIndex, Camera* camera) const;
	void BindToCommandPool(std::vector<VkCommandBuffer>& commandBuffers, int index) const;
	bool IsDebugEntity() const;

private:
	bool isDisposed = false;
	bool debugEntity = false;

	RenderEntityMeta* renderEntityMeta;
	RenderDataVulkanContext* renderDataVulkanContext;
	RenderDataCore* renderDataCore;
	RenderDataSingular* renderDataSingular;
	RenderDataInstanced* renderDataInstanced;

	void UpdateInstanceBuffer(uint32_t imageIndex, Camera* camera) const;
	void UpdateUniformBuffer(uint32_t imageIndex, Camera* camera) const;
	void DisposePipeline() const;
};
