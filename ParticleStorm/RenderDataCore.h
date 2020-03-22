#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "RenderMode.h"
#include "TransformEntity.h"

struct RenderDataCore {
	RenderMode renderMode;
	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;
	VkBuffer vertexBuffer;
	VkBuffer indexBuffer;
	uint32_t indexCount;
};

