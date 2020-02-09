#pragma once

#include <vec2.hpp>

struct RenderTransform {
	int objectCount; //TODO: Duplicate with RenderDataInstanced.instanceCount?
	bool staticScale;

	glm::vec2* pos;
	glm::vec2* scale;
};

