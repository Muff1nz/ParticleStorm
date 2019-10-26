#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "VulkanContext.h"

class VulkanAllocator {
public:
	VulkanAllocator(VulkanContext* vulkanContext);
	~VulkanAllocator();

	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) const;
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;	
	void Destroy(VkBuffer& buffer, VkDeviceMemory& bufferMemory) const;

	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
private:
	VulkanContext* vulkanContext;
};

