#pragma once

#include "GameEntity.h"

struct ParticlesEntity : GameEntity {
	ParticlesEntity() {
		radius = 1;
		type = ET_Particles;
	}

	float radius;
};
