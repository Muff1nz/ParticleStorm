#pragma once
#include <string>
#include "TransformEntity.h"

struct GraphicsEntity : TransformEntity {
	GraphicsEntity() {
		texturePath = "";
	}

	~GraphicsEntity() = default;

	std::string texturePath;
};
