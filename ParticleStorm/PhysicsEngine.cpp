#include "PhysicsEngine.h"
#include "Environment.h"
#include <detail/func_geometric.inl>
#include <thread>
#include <iostream>
#include "Timer.h"

PhysicsEngine::PhysicsEngine(Environment* environment, Stats* stats) : doubleRadius(environment->circleRadius * 2) {
	this->environment = environment;
	this->stats = stats;
	rng = NumberGenerator(environment->seed);
}

PhysicsEngine::~PhysicsEngine() = default;

void PhysicsEngine::Init() {
	const int maxSpeed = 1000;
	const auto circlePos = environment->circlePos;
	const auto circleVel = environment->circleVel;

	for (int i = 0; i < environment->circleCount; i++) {
		circlePos[i] = glm::vec2(rng.GenerateFloat(0, environment->worldWidth), rng.GenerateFloat(0, environment->worldHeight));

		do {
			circleVel[i] = glm::vec2(rng.GenerateFloat(-maxSpeed, maxSpeed), rng.GenerateFloat(-maxSpeed, maxSpeed));
		} while (abs(circleVel[i].x) < 1 && abs(circleVel[i].y) < 1);
	}

	environment->tree = new QuadTree(environment, Rect(0, 0, environment->worldWidth, environment->worldHeight));
}

void PhysicsEngine::Join() {
	physicsThread.join();
}

void PhysicsEngine::Start(bool* done) {
	physicsThread = std::thread([=] {PhysicsThreadRun(done);});
}

void PhysicsEngine::BoundingBoxCollision(const int particle) const {
	const auto circlePos = environment->circlePos;
	const auto circleVel = environment->circleVel;

	if (circlePos[particle].y < environment->circleRadius) {
		circleVel[particle].y = -circleVel[particle].y * friction;
		circlePos[particle].y = environment->circleRadius;
	}
	if (circlePos[particle].y > environment->worldHeight - environment->circleRadius) {
		circleVel[particle].y = -circleVel[particle].y * friction;
		circlePos[particle].y = environment->worldHeight - environment->circleRadius;
	}
	if (circlePos[particle].x < environment->circleRadius) {
		circleVel[particle].x = -circleVel[particle].x * friction;
		circlePos[particle].x = environment->circleRadius;
	}
	if (circlePos[particle].x > environment->worldWidth - environment->circleRadius) {
		circleVel[particle].x = -circleVel[particle].x * friction;
		circlePos[particle].x = environment->worldWidth - environment->circleRadius;
	}
}

void PhysicsEngine::ParticleCollision(const int particle1, const int particle2) const {
	const auto circlePos = environment->circlePos;
	const auto circleVel = environment->circleVel;

	const auto dist = distance(circlePos[particle1], circlePos[particle2]);
	if (dist < doubleRadius) {
		glm::vec2 positionDelta = circlePos[particle1] - circlePos[particle2];

		glm::vec2 newVel1 = circleVel[particle1] - glm::dot(circleVel[particle1] - circleVel[particle2], positionDelta) / pow(glm::length(positionDelta), 2) * positionDelta;

		positionDelta = -positionDelta;
		glm::vec2 newVel2 = circleVel[particle2] - glm::dot(circleVel[particle2] - circleVel[particle1], positionDelta) / pow(glm::length(positionDelta), 2) * positionDelta;

		circleVel[particle1] = newVel1 * friction;
		circleVel[particle2] = newVel2 * friction;

		const float overlap = environment->circleRadius * 2 - dist;
		circlePos[particle1] += -(positionDelta / dist) * (overlap / 2);
		circlePos[particle2] += (positionDelta / dist) * (overlap / 2);
		++stats->particleCollisionTotalLastSecond;
	}
}

void PhysicsEngine::ParticleCollision(const int particle) const {
	for (int i = particle + 1; i < environment->circleCount; ++i) {
		ParticleCollision(particle, i);
	}
}

void PhysicsEngine::ParticleCollision(const int particle, const int end, const std::vector<int>& overflow) const {
	const auto circlePos = environment->circlePos;
	const auto circleVel = environment->circleVel;

	//Overflow particles
	for (int i : overflow) 
		if (particle != i)
			ParticleCollision(particle, i);

	//In scope particles
	for (auto i = particle + 1; i < end; i++)
		ParticleCollision(particle, i);	
}

void PhysicsEngine::ParticleCollision(const int particle, const std::vector<int>& overflow) const {

	for (int i = particle + 1; i < overflow.size(); ++i) {
		ParticleCollision(overflow[particle], overflow[i]);
	}
	
}

void PhysicsEngine::QuadTreeParticleCollisions(QuadTree* tree) const {
	if (tree->subTree == nullptr) {
		for (int i = tree->start; i < tree->end; ++i) {
			ParticleCollision(i, tree->end, tree->overflow);
		}
		for (int i = 0; i < tree->overflow.size(); ++i) {
			ParticleCollision(i, tree->overflow);
		}
	} else {
		for (int i = 0; i < 4; ++i) {
			QuadTreeParticleCollisions(tree->subTree[i]);
		}
	}
}

void PhysicsEngine::PhysicsThreadRun(const bool* done) const {
	auto const explosionForce = 250000.0f;

	const auto circlePos = environment->circlePos;
	const auto circleVel = environment->circleVel;

	Timer timer(maxPhysicsDeltaTime, minPhysicsDeltaTime);

	while (!*done) {

		float deltaTime = timer.DeltaTime();
		stats->physicsTimeRatioTotalLastSecond += timer.RealTimeDifference();

		while (!environment->explosions.empty()) {
			glm::vec2 impactPoint = environment->explosions.front();
			for (int i = 0; i < environment->circleCount; i++) {
				glm::vec2 lineBetween = circlePos[i] - impactPoint;
				const float distance = length(lineBetween);
				lineBetween /= distance;
				circleVel[i] += lineBetween / distance * explosionForce;
			}
			environment->explosions.pop();

		}

		for (int i = 0; i < environment->circleCount; i++) {
			circleVel[i] += gravity * deltaTime;
			circlePos[i] += circleVel[i] * deltaTime;
		}

		int start = 0;
		environment->renderLock.lock();
		environment->treeMutex.lock();
		environment->tree->Build(nullptr, start, *stats);
		environment->treeMutex.unlock();
		environment->renderLock.unlock();

		for (int i = 0; i < environment->circleCount; i++) {
			BoundingBoxCollision(i);
			//ParticleCollision(i); //Non-Quadtree collisions
		}

		QuadTreeParticleCollisions(environment->tree);

		++stats->physicsUpdateTotalLastSecond;
		
	}
} 