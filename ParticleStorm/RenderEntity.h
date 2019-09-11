#pragma once
#include "RenderDataCore.h"
#include "RenderDataSingular.h"
#include "RenderDataInstanced.h"
#include "RenderDataVulkanContext.h"
#include "Environment.h"

class RenderEntity {
public:
	RenderEntity(RenderDataVulkanContext* renderDataVulkanContext, RenderDataCore* renderDataCore, RenderDataSingular* renderDataSingular, RenderDataInstanced* renderDataInstanced, bool debugEntity);
	~RenderEntity();

	void Dispose();

	void UpdateBuffers(uint32_t imageIndex, Camera* camera) const;
	void BindToCommandPool(std::vector<VkCommandBuffer>& commandBuffers, int index) const;
	bool IsDebugEntity() const;

private:
	bool isDisposed = false;
	bool debugEntity = false;

	RenderDataVulkanContext* renderDataVulkanContext;
	RenderDataCore* renderDataCore;
	RenderDataSingular* renderDataSingular;
	RenderDataInstanced* renderDataInstanced;

	void UpdateInstanceBuffer(uint32_t imageIndex, Camera* camera) const;
	void UpdateUniformBuffer(uint32_t imageIndex, Camera* camera) const;
};
