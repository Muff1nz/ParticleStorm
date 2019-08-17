#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

struct RenderDataInstanced {
	RenderDataInstanced();
	~RenderDataInstanced();

	std::vector<VkBuffer> instanceBuffers;
	std::vector<VkDeviceMemory> instanceMemory;
};

