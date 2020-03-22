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

	bool* particleResting{};

	//Why is this here?
	std::atomic_int* particleQuadCount{};

	//Debugging domain //Dunno how to handle yet
	int debugQuadSize = 1000;
	glm::vec2* quadPos;
	glm::vec2* quadScale;
};