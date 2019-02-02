#include "PhysicsEngine.h"
#include "Environment.h"
#include <detail/func_geometric.inl>
#include <thread>
#include <iostream>
#include "Timer.h"

PhysicsEngine::PhysicsEngine(Environment* environment, Stats* stats) : doubleRadius(environment->particleRadius * 2) {
	this->environment = environment;
	this->stats = stats;
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

	environment->tree = new QuadTree(environment, Rect(0, 0, environment->worldWidth, environment->worldHeight));
}

void PhysicsEngine::Join() {
	LeadThread.join();
}

void PhysicsEngine::Start(bool* done) {
	LeadThread = std::thread([=] {LeadThreadRun(done);});
}

void PhysicsEngine::BoundingBoxCollision(const int particle) const {
	const auto circlePos = environment->particlePos;
	const auto circleVel = environment->particleVel;

	if (circlePos[particle].y < environment->particleRadius) {
		circleVel[particle].y = -circleVel[particle].y * friction;
		circlePos[particle].y = environment->particleRadius;
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

void PhysicsEngine::ParticleCollision(const int particle1, const int particle2) const {
	const auto particlePos = environment->particlePos;
	const auto particleVel = environment->particleVel;
	//environment->LockParticles(particle1, particle2);

	const auto dist = distance(particlePos[particle1], particlePos[particle2]);
	if (dist < doubleRadius) {
		glm::vec2 positionDelta = particlePos[particle1] - particlePos[particle2];

		glm::vec2 newVel1 = particleVel[particle1] - dot(particleVel[particle1] - particleVel[particle2], positionDelta) / pow(length(positionDelta), 2) * positionDelta;

		positionDelta = -positionDelta;
		glm::vec2 newVel2 = particleVel[particle2] - dot(particleVel[particle2] - particleVel[particle1], positionDelta) / pow(length(positionDelta), 2) * positionDelta;

		particleVel[particle1] = newVel1 * friction;
		particleVel[particle2] = newVel2 * friction;

		const float overlap = environment->particleRadius * 2 - dist;
		particlePos[particle1] += -(positionDelta / dist) * (overlap / 2);
		particlePos[particle2] += (positionDelta / dist) * (overlap / 2);
		++stats->particleCollisionTotalLastSecond;
	}
	//environment->UnlockParticles(particle1, particle2);
}

void PhysicsEngine::ParticleCollision(const int particle) const {
	for (int i = particle + 1; i < environment->particleCount; ++i) {
		ParticleCollision(particle, i);
	}
}

void PhysicsEngine::ParticleCollision(const int particle, const int end, const std::vector<int>& overflow) const {
	const auto circlePos = environment->particlePos;
	const auto circleVel = environment->particleVel;

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

void PhysicsEngine::QuadTreeParticleCollisions(const QuadTree& tree) const {
	for (int i = 0; i < tree.overflow.size(); ++i) {
		ParticleCollision(i, tree.overflow);
	}
	for (int i = tree.start; i < tree.end; ++i) {
		ParticleCollision(i, tree.end, tree.overflow);
	}
}

void PhysicsEngine::UpdateParticles(int start, int end, float deltaTime) const {
	for (int i = start; i < end; i++) {
		environment->particleVel[i] += gravity * deltaTime;
		environment->particlePos[i] += environment->particleVel[i] * deltaTime;
	}
}

void PhysicsEngine::BoundingBoxCollisions(int start, int end) {
	for (int i = 0; i < environment->particleCount; i++) {
		BoundingBoxCollision(i);
	}
}

void PhysicsEngine::LeadThreadRun(bool* done) {
	auto const explosionForce = 250000.0f;

	const auto circlePos = environment->particlePos;
	const auto circleVel = environment->particleVel;

	Timer timer(maxPhysicsDeltaTime, minPhysicsDeltaTime);

	int particlesPerThread = environment->particleCount / workerThreadCount;
	auto particleSections = new std::tuple<int, int>[workerThreadCount];
	for (int i = 0; i < workerThreadCount + 1; ++i) {
		int start = i * particlesPerThread;
		int end = (i + 1) * particlesPerThread;
		end = end <= environment->particleCount ? end : environment->particleCount;
		particleSections[i] = { start, end };
	}

	workerThreads.Init(workerThreadCount, done);
	while (!*done) {

		float deltaTime = timer.DeltaTime();
		stats->physicsTimeRatioTotalLastSecond += timer.RealTimeDifference();

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

		for (int i = 0; i < workerThreadCount + 1; ++i) {		
			workerThreads.AddWork([=] { UpdateParticles(std::get<0>(particleSections[i]), std::get<1>(particleSections[i]), deltaTime); });
		}
		workerThreads.JoinWorkerThreads();

		environment->renderLock.lock();
		auto quads = environment->tree->Build(*stats);
		environment->renderLock.unlock();

		for (int i = 0; i < workerThreadCount + 1; ++i) {
			workerThreads.AddWork([=] { BoundingBoxCollisions(std::get<0>(particleSections[i]), std::get<1>(particleSections[i])); });
		}
		workerThreads.JoinWorkerThreads();

		for (QuadTree quad : quads) {
			workerThreads.AddWork([=] {QuadTreeParticleCollisions(quad); });
		}
		workerThreads.JoinWorkerThreads();

		++stats->physicsUpdateTotalLastSecond;		
	}

	workerThreads.CloseWorkerThreads();
	delete particleSections;
}