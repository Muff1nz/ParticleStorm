#pragma once
#include <SDL2/SDL.h>
#include "Environment.h"
#include <thread>
#include "Stats.h"

class PhysicsEngine {
public:
	PhysicsEngine(Environment* environment, Stats* stats);
	~PhysicsEngine();

	void Init() const;
	void Start(SDL_bool* done);
	void Join();
private:
	const float maxPhysicsDeltaTime = 1000.0f / 450.0f; //Which gives a minimum of 300 physics updates per "second" (maybe scale with particle radius)
	const glm::vec2 gravity = glm::vec2(0, -500);
	const float friction = 0.98;
	const float doubleRadius;

	Environment* environment;
	Stats* stats;

	std::thread physicsThread;

	bool ParticleCollision(const int particle1, const int particle2) const;
	void ParticleCollision(const int particle) const;
	int ParticleCollision(const int particle, const int end, const std::vector<int>& overflow) const;
	void ParticleCollision(const int particle, const std::vector<int>& overflow) const;
	void QuadTreeParticleCollisions(QuadTree* tree) const;

	bool BoundingBoxCollision(int particle) const;
	void PhysicsThreadRun(const SDL_bool* done) const;
};

