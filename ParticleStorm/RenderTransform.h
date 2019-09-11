#pragma once

#include <vec2.hpp>

struct RenderTransform {
	int objectCount;
	bool staticScale;

	glm::vec2* pos;
	glm::vec2* scale;
};

