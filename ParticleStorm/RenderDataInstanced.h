#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include "InstanceBufferObject.h"

struct RenderDataInstanced {
	~RenderDataInstanced() = default;

	std::vector<VkBuffer> instanceBuffers;
	std::vector<VkDeviceMemory> instanceMemory;
	InstanceBufferObject* instanceBufferObjects;
	int instanceCount;	
};

