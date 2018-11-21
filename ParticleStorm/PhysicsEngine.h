#pragma once
#include <SDL2/SDL.h>
#include "Environment.h"
#include <thread>

class PhysicsEngine {
public:
	PhysicsEngine(Environment* environment);
	~PhysicsEngine();

	void Init() const;
	void Start(SDL_bool* done, int* physicsUpdates, QuadTree::Stats* stats);
	void Join();
private:
	const int maxPhysicsUpdatesPerSecond = 3000000;
	const glm::vec2 gravity = glm::vec2(0, -0.001);
	const float friction = 0.98;
	const float doubleRadius;

	Environment* environment;

	std::thread physicsThread;

	bool ParticleCollision(const int particle1, const int particle2) const;
	void ParticleCollision(const int particle) const;
	int ParticleCollision(const int particle, const int end, const std::vector<int>& overflow) const;
	void ParticleCollision(const int particle, const std::vector<int>& overflow) const;
	void QuadTreeParticleCollisions(QuadTree* tree) const;

	bool BoundingBoxCollision(int particle) const;
	void PhysicsThreadRun(const SDL_bool* done, int* physicsUpdate, QuadTree::Stats* statss) const;
};

