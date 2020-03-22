#pragma once
#include <string>
#include "TransformEntity.h"

struct GameEntity : TransformEntity {
	GameEntity() {
		texturePath = "";
		velocity = nullptr;
	}

	~GameEntity(){
		delete velocity;
	}

	std::string texturePath;
	glm::vec2* velocity{};
};
