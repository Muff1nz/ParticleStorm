#pragma once
#include "Environment.h"
#include <thread>
#include "Stats.h"
#include "NumberGenerator.h"
#include "ConcurrentQueue.h"

class PhysicsEngine {
public:
	PhysicsEngine(Environment* environment, Stats* stats);
	~PhysicsEngine();

	void Init();
	void Start(bool* done);
	void Join();
private:
	const float maxPhysicsDeltaTime = 1.0f / 450.0f; //Which gives a minimum of 450 physics updates per "second" (maybe scale with particle radius)
	const float minPhysicsDeltaTime = 1.0f / 700.0f; 
	const glm::vec2 gravity = glm::vec2(0, -500);
	const float friction = 0.98;
	const float doubleRadius;

	const int WorkerThreadCount = 4;

	NumberGenerator rng;

	Environment* environment;
	Stats* stats;

	std::thread LeadThread;
	std::vector<std::thread> WorkerThreads;
	ConcurrentQueue<std::function<void()>> work;

	void ParticleCollision(int particle1, int particle2) const;
	void ParticleCollision(int particle) const;
	void ParticleCollision(int particle, int end, const std::vector<int>& overflow) const;
	void ParticleCollision(int particle, const std::vector<int>& overflow) const;
	void QuadTreeParticleCollisions(QuadTree* tree) const;

	void BoundingBoxCollision(int particle) const;
	void PhysicsThreadRun(const bool* done) const;
};

