#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include "InstanceBufferObject.h"

struct RenderDataInstanced {
	std::vector<VkBuffer> instanceBuffers;
	std::vector<VkDeviceMemory> instanceMemory;
	InstanceBufferObject* instanceBufferObjects;
	int instanceCount;	
};

