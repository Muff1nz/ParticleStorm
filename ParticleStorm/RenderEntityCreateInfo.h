#pragma once
#include <string>

#include "RenderMode.h"

struct RenderEntityCreateInfo {
	std::string vertexShader;
	std::string fragmentShader;

	std::string texturePath;

	RenderMode renderMode;

	VkBuffer vertexBuffer;
	VkBuffer indexBuffer;
	uint32_t indexCount;
};
