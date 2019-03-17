#pragma once
#include "Environment.h"
#include <thread>
#include "Stats.h"
#include "NumberGenerator.h"
#include "LinearQuad.h"

class PhysicsEngine {
public:
	PhysicsEngine(Environment* environment, Stats* stats);
	~PhysicsEngine();

	void Init();
	void Start();
	void Join();
private:
	const float maxPhysicsDeltaTime = 1.0f / 60.0f; //Which gives a minimum of 450 physics updates per "second" (maybe scale with particle radius)
	const float minPhysicsDeltaTime = 1.0f / 1000.0f; 
	const glm::vec2 gravity = glm::vec2(0, -500);
	const float friction = 0.99;
	const float doubleRadius;

	NumberGenerator rng;

	Environment* environment;
	Stats* stats;

	std::thread LeadThread;

	void ParticleCollision(int particle1, int particle2) const;
	void ParticleCollision(int particle, int end, const LazyVector<int>& overflow) const;
	void ParticleCollision(int particle, const LazyVector<int>& overflow) const;
	void LinearQuadTreeParticleCollisions(const LinearQuad& linearQuad) const;
	void LinearQuadTreeParticleCollisions(const LinearQuad* linearQuad) const;
	void LinearQuadTreeParticleCollisions(const LazyVector<LinearQuad>* linearQuads, int start, int end) const;
	void QuadTreeParticleCollisions(QuadTree* tree) const;
	void QuadTreeParticleCollisions(ConcurrentVector<QuadTree*>* quads, int start, int end) const;
	void UpdateParticles(int start, int end, float deltaTime) const;
	void HandleExplosions() const;
	void BoundingBoxCollision(int particle) const;

	void AlignQuadTree(ConcurrentVector<QuadTree*>& quads, LazyVector<LinearQuad>& linearQuads) const;
	void LeadThreadRun();
};

