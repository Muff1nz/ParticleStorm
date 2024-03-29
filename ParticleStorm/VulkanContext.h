#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

struct VulkanContext {
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;
	VkQueue graphicsQueue;
	VkRenderPass renderPass;
	VkExtent2D swapChainExtent;		
	std::vector<VkImage> swapChainImages;
	VkCommandPool commandPool;
	std::vector<VkFramebuffer> swapChainFrameBuffers;
	VkSwapchainKHR swapChain;
	VkQueue presentQueue;
};
