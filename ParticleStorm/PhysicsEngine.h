#pragma once
#include <SDL2/SDL.h>
#include "Environment.h"
#include <thread>

class PhysicsEngine {
public:
	PhysicsEngine(Environment* environment);
	~PhysicsEngine();

	void Init() const;
	void Start(SDL_bool* done, int* physicsUpdates);
	void Join();
private:
	const int maxPhysicsUpdatesPerSecond = 3000000;
	const glm::vec2 gravity = glm::vec2(0, -0.001);
	const float friction = 0.98;

	Environment* environment;

	std::thread physicsThread;

	bool BoundingBoxCollision(int particle) const;
	int ParticleCollision(int particle, float deltaTime) const;
	void PhysicsThreadRun(const SDL_bool* done, int* physicsUpdates) const;
};

