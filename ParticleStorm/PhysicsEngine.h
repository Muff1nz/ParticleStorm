#pragma once

#include <thread>

#include "LinearQuad.h"
#include "CollisionChecker.h"
#include "MessageSystem.h"
#include "PhysicsParticlesEntity.h"
#include "WorkerThreadPool.h"
#include "Stats.h"
#include "WorldEntity.h"
#include "QuadTreeHandler.h"
#include "DebugQuadTreeEntity.h"

class PhysicsEngine {
public:
	PhysicsEngine(MessageSystem* messageQueue, WorkerThreadPool* workerThreads, Stats* stats);
	~PhysicsEngine();

	void Start();
	void Join();

private:
	const double maxPhysicsDeltaTime = 1.0 / 60.0; //Which gives a minimum of 450 physics updates per "second" (maybe scale with particle radius)
	const glm::vec2 gravity = glm::vec2(0, -400);
	const float friction = 0.99f;

	PhysicsParticlesEntity* particles;
	WorldEntity* world;
	DebugQuadTreeEntity* debugQuadTree;

	QuadTreeHandler* quadTreeHandler;
	std::vector<Range> particleSections;

	WorkerThreadPool* workerThreads;
	Stats* stats;

	MessageSystem* messageQueue;

	bool shouldRun;

	std::thread LeadPhysicsThread;
	CollisionChecker collisionChecker;
	
	void AddEntity(Message message);
	void RemoveEntity(Message message);

	void HandleMessages();
	void HandleExplosion(Message message) const;	

	void ResolveCollision(int particle1, int particle2, float dist) const;
	void QuadInternalParticleCollision(int localParticle1, int localParticle2, LinearQuad* tree) const;
	void QuadMixedParticleCollision(int localParticle1, int localParticle2, LinearQuad* tree) const;
	void QuadExternalCollision(int localParticle1, int localParticle2, LinearQuad* tree) const;
	void LinearQuadParticleCollisions(LinearQuad* tree) const;
	void LinearQuadParticleCollisions(std::vector<LinearQuad*>* quads, int start, int end) const;
	void UpdateParticles(int start, int end, float deltaTime) const;
	void WorldBoundsCheck(int particle) const;

	void LeadThreadRun();
};

