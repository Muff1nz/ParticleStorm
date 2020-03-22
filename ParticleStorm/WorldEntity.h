#pragma once

#include "GameEntity.h"

struct WorldEntity : GameEntity {
	WorldEntity() {
		height = 100;
		width = 100;
		type = ET_World;
	}

	int height;
	int width;
};
