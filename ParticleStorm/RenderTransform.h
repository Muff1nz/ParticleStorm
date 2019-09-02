#pragma once

#include <vec2.hpp>

struct RenderTransform {
	int instanceCount;
	glm::vec2* pos;
	glm::vec2 scale;
};

