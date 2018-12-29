#pragma once
#include <vec2.hpp>

class TestSquare {
public:
	glm::vec2 pos;
	glm::vec2 scale;

	TestSquare() {
		pos = { 0, 0 };
		scale = { 1, 1 };
	}
};
