#pragma once
#include <vec2.hpp>
#include "BaseEntity.h"

struct TransformEntity : BaseEntity {
	TransformEntity() {
		count = 1;
		uniformScale = true;

		position = nullptr;
		scale = nullptr;
	}

	~TransformEntity() {
		delete position;
		delete scale;
	}

	int count;
	bool uniformScale;

	glm::vec2* position{};
	glm::vec2* scale{};
};
