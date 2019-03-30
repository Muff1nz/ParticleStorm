#pragma once
#include "Environment.h"
#include <thread>
#include "Stats.h"
#include "NumberGenerator.h"

class PhysicsEngine {
public:
	PhysicsEngine(Environment* environment, Stats* stats);
	~PhysicsEngine();

	void Init();
	void Start();
	void Join();
private:
	const float maxPhysicsDeltaTime = 1.0f / 450.0f; //Which gives a minimum of 450 physics updates per "second" (maybe scale with particle radius)
	const float minPhysicsDeltaTime = 1.0f / 700.0f; 
	const glm::vec2 gravity = glm::vec2(0, -500);
	const float friction = 0.99;
	const float doubleRadius;

	NumberGenerator rng;

	Environment* environment;
	Stats* stats;

	std::thread LeadThread;

	void ParticleCollision(int particle1, int particle2) const;
	void QuadTreeParticleCollisions(const QuadTree* tree) const;
	void QuadTreeParticleCollisions(ConcurrentVectror<QuadTree*>* quads, int start, int end) const;
	void UpdateParticles(int start, int end, float deltaTime) const;
	void HandleExplosions() const;
	void BoundingBoxCollision(int particle) const;

	void LeadThreadRun();
};

