#include "PhysicsEngine.h"
#include <detail/func_geometric.inl>
#include <iostream>
#include "Timer.h"
#include "Range.h"
#include <map>
#include <common.hpp>
#include <common.hpp>
#include <common.hpp>
#include <common.hpp>

PhysicsEngine::PhysicsEngine(Environment* environment) : doubleRadius(environment->particleRadius * 2) {
	this->environment = environment;
	rng = NumberGenerator(environment->seed);
}

PhysicsEngine::~PhysicsEngine() = default;

void PhysicsEngine::Init() {
	const int maxSpeed = 1000;
	const auto circlePos = environment->particlePos;
	const auto circleVel = environment->particleVel;

	for (int i = 0; i < environment->particleCount; i++) { //Initialize particles (position/velocity)
		circlePos[i] = glm::vec2(rng.GenerateFloat(0, environment->worldWidth), rng.GenerateFloat(0, environment->worldHeight));

		do {
			circleVel[i] = glm::vec2(rng.GenerateFloat(-maxSpeed, maxSpeed), rng.GenerateFloat(-maxSpeed, maxSpeed));
		} while (abs(circleVel[i].x) < 1 && abs(circleVel[i].y) < 1);
	}

	environment->tree = new QuadTree(nullptr, environment, Rect(0, 0, environment->worldWidth, environment->worldHeight));
}

void PhysicsEngine::Join() {
	LeadThread.join();
}

void PhysicsEngine::Start() {
	LeadThread = std::thread([=] {LeadThreadRun();});
}

void PhysicsEngine::BoundingBoxCollision(const int particle) const {
	const auto circlePos = environment->particlePos;
	const auto circleVel = environment->particleVel;

	if (circlePos[particle].y < environment->particleRadius) {
		circleVel[particle].y = -circleVel[particle].y * friction;
		circlePos[particle].y = environment->particleRadius;
		environment->particleResting[particle] = true;
	}
	if (circlePos[particle].y > environment->worldHeight - environment->particleRadius) {
		circleVel[particle].y = -circleVel[particle].y * friction;
		circlePos[particle].y = environment->worldHeight - environment->particleRadius;
	}
	if (circlePos[particle].x < environment->particleRadius) {
		circleVel[particle].x = -circleVel[particle].x * friction;
		circlePos[particle].x = environment->particleRadius;
	}
	if (circlePos[particle].x > environment->worldWidth - environment->particleRadius) {
		circleVel[particle].x = -circleVel[particle].x * friction;
		circlePos[particle].x = environment->worldWidth - environment->particleRadius;
	}
}


void PhysicsEngine::ResolveCollision(const int particle1, const int particle2, const float dist) const {
	const auto particlePos = environment->particlePos;
	const auto particleVel = environment->particleVel;
	const auto particleResting = environment->particleResting;

	glm::vec2 positionDelta = particlePos[particle1] - particlePos[particle2];
	const float overlap = environment->particleRadius * 2 - dist;
	const glm::vec2 displacement = positionDelta / dist * (overlap / 2);
	particlePos[particle1] += displacement;
	particlePos[particle2] += -displacement;
	positionDelta = particlePos[particle1] - particlePos[particle2];

	const glm::vec2 newVel1 = particleVel[particle1] - dot(particleVel[particle1] - particleVel[particle2], positionDelta) / pow(length(positionDelta), 2) * positionDelta;

	positionDelta = -positionDelta;
	const glm::vec2 newVel2 = particleVel[particle2] - dot(particleVel[particle2] - particleVel[particle1], positionDelta) / pow(length(positionDelta), 2) * positionDelta;

	particleVel[particle1] = newVel1 * friction;
	particleVel[particle2] = newVel2 * friction;

	if (particlePos[particle1].y >= particlePos[particle2].y)
		particleResting[particle1] = true;
	else
		particleResting[particle2] = true;
}

void PhysicsEngine::QuadInternalParticleCollision(const int particle1, const int particle2, QuadTree* tree) const {
	const auto dist = distance(tree->particlePos[particle1], tree->particlePos[particle2]);
	if (dist < doubleRadius) {
		const int globalParticle1 = tree->particleIndex[particle1];
		const int globalParticle2 = tree->particleIndex[particle2];
		ResolveCollision(globalParticle1, globalParticle2, dist);
		tree->particlePos[particle1] = environment->particlePos[globalParticle1];
		tree->particlePos[particle2] = environment->particlePos[globalParticle2];
		++environment->stats.particleCollisionTotalLastSecond;
	}
}

void PhysicsEngine::QuadMixedParticleCollision(const int particle1, const int particle2, QuadTree* tree) const {
	const int globalParticle2 = tree->particleIndex[particle2];
	const auto dist = distance(tree->particlePos[particle1], environment->particlePos[globalParticle2]);
	if (dist < doubleRadius) {
		const int globalParticle1 = tree->particleIndex[particle1];
		ResolveCollision(globalParticle1, globalParticle2, dist);
		tree->particlePos[particle1] = environment->particlePos[globalParticle1];
		++environment->stats.particleCollisionTotalLastSecond;
	}
}


void PhysicsEngine::QuadExternalCollision(const int particle1, const int particle2) const {
	const auto dist = distance(environment->particlePos[particle1], environment->particlePos[particle2]);
	if (dist < doubleRadius) {
		ResolveCollision(particle1, particle2, dist);
		++environment->stats.particleCollisionTotalLastSecond;
	}
}

void PhysicsEngine::QuadTreeParticleCollisions(QuadTree* tree) const {
	for (int i = 0; i < tree->particleIndex.size(); ++i) { //All inscope against each other
		for (int j = i + 1; j < tree->particleIndex.size(); ++j) {
			const int globalI = tree->particleIndex[i];
			const int gloablJ = tree->particleIndex[j];

			if (environment->particleQuadCount[globalI] > 1 && environment->particleQuadCount[gloablJ] > 1)
				QuadExternalCollision(globalI, gloablJ);
			else if (environment->particleQuadCount[globalI] > 1)
				QuadMixedParticleCollision(j, i, tree);
			else if (environment->particleQuadCount[gloablJ] > 1)
				QuadMixedParticleCollision(i, j, tree);
			else
				QuadInternalParticleCollision(i, j, tree);
		}
	}

	//for (int i = 0; i < tree->overflow.size(); ++i) { //All inscope against each other
	//	for (int j = i + 1; j < tree->overflow.size(); ++j) {
	//		QuadInternalParticleCollision(tree->overflow[i], tree->overflow[j], tree);
	//	}
	//}
}

void PhysicsEngine::QuadTreeParticleCollisions(const int start, const int end) const {
	for (int i = start; i < end; ++i) {
		QuadTreeParticleCollisions(environment->quads[i]);
	}
}

void PhysicsEngine::UpdateParticles(int start, int end, float deltaTime) const {
	for (int i = start; i < end; i++) {
		BoundingBoxCollision(i);
		if (!environment->particleResting[i])
			environment->particleVel[i] += gravity * deltaTime;
		environment->particlePos[i] += environment->particleVel[i] * deltaTime;
		environment->particleResting[i] = false;
		environment->particleQuadCount[i] = 0;
	}
}

void PhysicsEngine::HandleExplosions() const {
	float const explosionForce = 250000.0f;
	const auto circlePos = environment->particlePos;
	const auto circleVel = environment->particleVel;

	while (!environment->explosions.empty()) {
		glm::vec2 impactPoint = environment->explosions.front();
		for (int i = 0; i < environment->particleCount; i++) {
			glm::vec2 lineBetween = circlePos[i] - impactPoint;
			const float distance = length(lineBetween);
			lineBetween /= distance;
			circleVel[i] += lineBetween / distance * explosionForce;
		}
		environment->explosions.pop();
	}
}


void PhysicsEngine::LeadThreadRun() {
	const auto circlePos = environment->particlePos;
	const auto circleVel = environment->particleVel;

	Timer timer(maxPhysicsDeltaTime, minPhysicsDeltaTime);

	std::vector<Range> particleSections;
	environment->workerThreads.PartitionForWorkers(environment->particleCount, particleSections);

	while (!environment->done) { //TODO: Currently have commented out linear quads code. Didn't seem like it was immediately better.

		float deltaTime = timer.DeltaTime();
		environment->stats.physicsTimeRatioTotalLastSecond += timer.RealTimeDifference();

		//EXPLOSIONS
		timer.Restart();
		HandleExplosions();
		timer.Stop();
		environment->stats.puEventsTotalLastSecond += timer.ElapsedMilliseconds();

		//QUADTREE
		timer.Restart();
		environment->tree->BuildRoot();
		environment->workerThreads.JoinWorkerThreads();
		timer.Stop();
		environment->stats.puQuadTreeUpdateTotalLastSecond += timer.ElapsedMilliseconds();

		//PARTICLE COLLISIONS
		timer.Restart();
		std::vector<Range> quadSections;
		environment->workerThreads.PartitionForWorkers(environment->quads.Size(), quadSections);
		for (auto quadSection : quadSections)
			environment->workerThreads.AddWork([=] { QuadTreeParticleCollisions(quadSection.lower, quadSection.upper); });
		environment->workerThreads.JoinWorkerThreads();
		timer.Stop();
		environment->stats.puCollisionUpdateTotalLastSecond += timer.ElapsedMilliseconds();

		//UPDATES
		timer.Restart();
		for (auto particleSection : particleSections)
			environment->workerThreads.AddWork([=] { UpdateParticles(particleSection.lower, particleSection.upper, deltaTime); });
		environment->workerThreads.JoinWorkerThreads();
		timer.Stop();
		environment->stats.puPositionUpdatesTotalLastSecond += timer.ElapsedMilliseconds();

		++environment->stats.physicsUpdateTotalLastSecond;
	}

	std::cout << "\nJOINING THE THREADS!!\n";
	environment->workerThreads.CloseWorkerThreads();
	std::cout << "\nRUN COMPLETE!!\n";
}
