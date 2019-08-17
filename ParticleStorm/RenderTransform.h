#pragma once

#include <vec2.hpp>

struct RenderTransform {
	RenderTransform();
	~RenderTransform();

	glm::vec2* pos;
	glm::vec2 scale;
};

