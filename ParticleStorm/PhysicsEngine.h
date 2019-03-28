#pragma once
#include "Environment.h"
#include <thread>
#include "NumberGenerator.h"

class PhysicsEngine {
public:
	PhysicsEngine(Environment* environment);
	~PhysicsEngine();

	void Init();
	void Start();
	void Join();
private:
	const float maxPhysicsDeltaTime = 1.0f / 250.0f; //Which gives a minimum of 450 physics updates per "second" (maybe scale with particle radius)
	const float minPhysicsDeltaTime = 1.0f / 1000.0f; 
	const glm::vec2 gravity = glm::vec2(0, -500);
	const float friction = 0.99;
	const float doubleRadius;

	NumberGenerator rng;

	Environment* environment;

	std::thread LeadThread;

	void ResolveCollision(int particle1, int particle2, float dist) const;
	void QuadInternalParticleCollision(const int localParticle1, const int localParticle2, QuadTree* tree) const;
	void QuadMixedParticleCollision(const int localParticle1, const int localParticle2, QuadTree* tree) const;
	void QuadExternalCollision(const int localParticle1, const int localParticle2, QuadTree* tree) const;
	void QuadTreeParticleCollisions(QuadTree* tree) const;
	void QuadTreeParticleCollisions(int start, int end) const;
	void UpdateParticles(int start, int end, float deltaTime) const;
	void HandleExplosions() const;
	void BoundingBoxCollision(int particle) const;

	void LeadThreadRun();
};

