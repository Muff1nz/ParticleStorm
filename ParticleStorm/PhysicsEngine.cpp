#include "PhysicsEngine.h"
#include "Environment.h"
#include <detail/func_geometric.inl>
#include <thread>
#include <iostream>

PhysicsEngine::PhysicsEngine(Environment* environment) {
	this->environment = environment;
}

PhysicsEngine::~PhysicsEngine() = default;

void PhysicsEngine::Init() const {
	const int maxSpeed = 1;
	const auto circlePos = environment->circlePos;
	const auto circleVel = environment->circleVel;

	for (int i = 0; i < environment->circleCount; i++) {
		circlePos[i] = glm::vec2(rand() % environment->worldWidth, rand() % environment->worldHeight);

		do {
			circleVel[i] = glm::vec2(rand() % (maxSpeed * 2) - maxSpeed, rand() % (maxSpeed * 2) - maxSpeed);
		} while (abs(circleVel[i].x) < 1 && abs(circleVel[i].y) < 1);
	}
}

void PhysicsEngine::Join() {
	physicsThread.join();
}

void PhysicsEngine::Start(SDL_bool* done, int* physicsUpdates) {
	physicsThread = std::thread([=] {PhysicsThreadRun(done, physicsUpdates);});
}

bool PhysicsEngine::BoundingBoxCollision(const int particle) const {
	bool collision = false;
	const auto circlePos = environment->circlePos;
	const auto circleVel = environment->circleVel;

	if (circlePos[particle].y < environment->circleRadius) {
		circleVel[particle].y = -circleVel[particle].y * friction;
		circlePos[particle].y = environment->circleRadius;
		collision = true;
	}
	if (circlePos[particle].y > environment->worldHeight - environment->circleRadius) {
		circleVel[particle].y = -circleVel[particle].y * friction;
		circlePos[particle].y = environment->worldHeight - environment->circleRadius;
		collision = true;
	}
	if (circlePos[particle].x < environment->circleRadius) {
		circleVel[particle].x = -circleVel[particle].x * friction;
		circlePos[particle].x = environment->circleRadius;
		collision = true;
	}
	if (circlePos[particle].x > environment->worldWidth - environment->circleRadius) {
		circleVel[particle].x = -circleVel[particle].x * friction;
		circlePos[particle].x = environment->worldWidth - environment->circleRadius;
		collision = true;
	}
	return collision;
}

int PhysicsEngine::ParticleCollision(const int particle, float deltaTime) const {
	const auto circlePos = environment->circlePos;
	const auto circleVel = environment->circleVel;

	int collisionCount = 0;
	for (int i = particle + 1; i < environment->circleCount; i++) {
		const float distance = glm::distance(circlePos[particle], circlePos[i]);
		if (distance < environment->circleRadius * 2) {
			collisionCount++;

			glm::vec2 positionDelta = circlePos[particle] - circlePos[i];

			glm::vec2 newVel1 = circleVel[particle] - glm::dot(circleVel[particle] - circleVel[i], positionDelta) / pow(glm::length(positionDelta), 2) * positionDelta;

			positionDelta = -positionDelta;
			glm::vec2 newVel2 = circleVel[i] - glm::dot(circleVel[i] - circleVel[particle], positionDelta) / pow(glm::length(positionDelta), 2) * positionDelta;

			circleVel[particle] = newVel1 * friction;
			circleVel[i] = newVel2 * friction;

			const float overlap = environment->circleRadius * 2 - distance;
			circlePos[particle] += -(positionDelta / distance) * (overlap / 2);
			circlePos[i] += (positionDelta / distance) * (overlap / 2);
		}
	}
	return collisionCount;
}

void PhysicsEngine::PhysicsThreadRun(const SDL_bool* done, int* physicsUpdates) const {
	Uint64 NOW = SDL_GetPerformanceCounter();
	Uint64 LAST = NOW;
	auto const explosionForce = 250.0f;

	const auto circlePos = environment->circlePos;
	const auto circleVel = environment->circleVel;

	while (!*done) {
		NOW = SDL_GetPerformanceCounter();
		const auto deltaTime = float((NOW - LAST) * 1000 / float(SDL_GetPerformanceFrequency()));
		if (deltaTime > 1000.0f / maxPhysicsUpdatesPerSecond) {
			LAST = NOW;

			while (!environment->explosions.empty()) {
				std::cout << "Applying boom!\n";
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

			for (int i = 0; i < environment->circleCount; i++) {
				int collisionCount = BoundingBoxCollision(i) ? 1 : 0;
				collisionCount += ParticleCollision(i, deltaTime);
			}

			(*physicsUpdates)++;
		}
	}
} 