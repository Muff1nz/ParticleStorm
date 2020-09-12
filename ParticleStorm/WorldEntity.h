#pragma once

#include "GameEntity.h"
#include "IdGenerator.h"

struct WorldEntity : GameEntity {
	WorldEntity() {
		type = ET_World;
		id = IdGenerator::GetNewId();
		
		height = 100;
		width = 100;
	}

	WorldEntity(int width, int height) {
		id = IdGenerator::GetNewId();
		type = ET_World;
		
		this->width = width;
		this->height = height;
		position = new glm::vec2(width / 2, height / 2);
		scale = new glm::vec2(width / 2, height / 2);
		texturePath = "Textures/BackGround.png";
	}

	int height;
	int width;
};
