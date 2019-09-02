#pragma once
#include "RenderDataCore.h"
#include "RenderDataSingular.h"
#include "RenderDataInstanced.h"
#include "RenderDataVulkanContext.h"
#include "Environment.h"

class RenderEntity {
public:
	std::string name{};

	RenderEntity(RenderDataVulkanContext* renderDataVulkanContext, RenderDataCore* renderDataCore, RenderDataSingular* renderDataSingular, RenderDataInstanced* renderDataInstanced);
	~RenderEntity();

	void Dispose();

	void UpdateBuffers(uint32_t imageIndex, Camera* camera) const;
	void BindToCommandPool(std::vector<VkCommandBuffer> &commandBuffers, VkBuffer &quadVertexBuffer, VkBuffer &quadIndexBuffer, const std::vector<uint16_t> &indices, int index) const;
private:
	bool isDisposed = false;

	RenderDataVulkanContext* renderDataVulkanContext;
	RenderDataCore* renderDataCore;
	RenderDataSingular* renderDataSingular;
	RenderDataInstanced* renderDataInstanced;

	void UpdateInstanceBuffer(uint32_t imageIndex, Camera* camera) const;
	void UpdateUniformBuffer(uint32_t imageIndex, Camera* camera) const;
};
