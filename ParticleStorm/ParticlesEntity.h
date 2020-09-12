#pragma once

#include "GameEntity.h"
#include "IdGenerator.h"
#include "NumberGenerator.h"

struct ParticlesEntity : GameEntity {
	ParticlesEntity() {
		id = IdGenerator::GetNewId();
		type = ET_Particles;
		
		radius = 1;
	}

	ParticlesEntity(int count, int radius) {
		id = IdGenerator::GetNewId();
		type = ET_Particles;

		this->count = count;
		this->radius = radius;
		position = new glm::vec2[count];
		velocity = new glm::vec2[count];
		scale = new glm::vec2(radius, radius);
		texturePath = "Textures/Circle.png";
	}

	void Initialize(int seed, int worldWidth, int worldHeight) const {
		auto rng = NumberGenerator(seed);

		const int maxSpeed = 1000;
		const auto circlePos = position;
		const auto circleVel = velocity;

		for (int i = 0; i < count; i++) { //Initialize particles (position/velocity)
			circlePos[i] = glm::vec2(rng.GenerateFloat(0, worldWidth), rng.GenerateFloat(0, worldHeight));

			do {
				circleVel[i] = glm::vec2(rng.GenerateFloat(-maxSpeed, maxSpeed), rng.GenerateFloat(-maxSpeed, maxSpeed));
			} while (abs(circleVel[i].x) < 1 && abs(circleVel[i].y) < 1);
		}
	}

	float radius;
};
