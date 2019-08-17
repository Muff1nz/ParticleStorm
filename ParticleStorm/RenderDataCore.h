#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "RenderTransform.h"

struct RenderDataCore {
	RenderDataCore();
	~RenderDataCore();

	RenderTransform transform;

	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;
};

