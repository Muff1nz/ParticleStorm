#pragma once
#include "RenderDataCore.h"
#include "RenderDataSingular.h"
#include "RenderDataInstanced.h"
#include "RenderDataVulkanContext.h"
#include "Environment.h"

class RenderEntity {
public:
	RenderEntity(RenderDataVulkanContext* renderDataVulkanContext, RenderDataCore* renderDataCore, RenderDataSingular* renderDataSingular, RenderDataInstanced* renderDataInstanced);
	~RenderEntity();

	void Dispose();

	void UpdateBuffers(uint32_t imageIndex, Environment* environment) const;
	void BindToCommandPool(std::vector<VkCommandBuffer> &commandBuffers, VkBuffer &quadVertexBuffer, VkBuffer &quadIndexBuffer, const std::vector<uint16_t> &indices, int index) const;
private:
	bool isDisposed = false;
	const bool isStatic = false;

	RenderDataVulkanContext* renderDataVulkanContext;
	RenderDataCore* renderDataCore;
	RenderDataSingular* renderDataSingular;
	RenderDataInstanced* renderDataInstanced;

	void UpdateInstanceBuffer(uint32_t imageIndex, Environment* environment) const;
	void UpdateUniformBuffer(uint32_t imageIndex, Environment* environment) const;
};
