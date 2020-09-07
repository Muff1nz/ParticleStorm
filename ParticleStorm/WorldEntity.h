#pragma once

#include "GameEntity.h"
#include "IdGenerator.h"

struct WorldEntity : GameEntity {
	WorldEntity() {
		height = 100;
		width = 100;
		type = ET_World;
	}

	WorldEntity(int width, int height) {
		type = ET_World;
		
		id = IdGenerator::GetNewId();
		this->width = width;
		this->height = height;
		position = new glm::vec2(width / 2, height / 2);
		scale = new glm::vec2(width / 2, height / 2);
		texturePath = "Textures/BackGround.png";
	}

	int height;
	int width;
};
