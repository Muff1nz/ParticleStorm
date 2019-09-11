#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "RenderTransform.h"
#include "RenderMode.h"

struct RenderDataCore {
	RenderMode renderMode;
	RenderTransform transform;
	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;
	VkBuffer vertexBuffer;
	VkBuffer indexBuffer;
	uint32_t indexCount;
};

