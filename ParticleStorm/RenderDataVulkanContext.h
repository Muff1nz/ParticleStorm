#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

struct  RenderDataVulkanContext {
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;
	VkQueue graphicsQueue;
	VkRenderPass renderPass;
	VkExtent2D swapChainExtent;		
	std::vector<VkImage> swapChainImages;
	VkCommandPool commandPool;
};
