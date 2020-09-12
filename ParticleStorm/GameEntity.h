#pragma once

#include "GraphicsEntity.h"

struct GameEntity : GraphicsEntity {
	GameEntity() {
		velocity = nullptr;
	}

	~GameEntity(){
		delete velocity;
	}

	glm::vec2* velocity{};
};
