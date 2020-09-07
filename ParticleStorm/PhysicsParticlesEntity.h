#pragma once
#include <vec2.hpp>
#include <atomic>
#include "ParticlesEntity.h"

struct PhysicsParticlesEntity {
	PhysicsParticlesEntity(ParticlesEntity* entity) {
		particlesEntity = entity;
		count = entity->count;
		radius = entity->radius;
		position = entity->position;
		velocity = entity->velocity;

		particleQuadCount = new std::atomic_int[count];
		particleResting = new bool[count];
	}

	~PhysicsParticlesEntity() {
		Dispose();
	}

	void Dispose() {
		delete particleQuadCount;
		delete particleResting;
	}

	ParticlesEntity* particlesEntity;

	int count;
	int radius;

	glm::vec2* position;
	glm::vec2* velocity;	

	//Physics engine specific technical data
	bool* particleResting{};
	std::atomic_int* particleQuadCount{};
};