#include "PhysicsEngine.h"
#include "Environment.h"
#include <detail/func_geometric.inl>
#include <thread>
#include <iostream>
#include "Timer.h"
#include "Range.h"

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

void PhysicsEngine::Start() {
	LeadThread = std::thread([=] {LeadThreadRun();});
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

void PhysicsEngine::ParticleCollisionsNonQuadTreee(const int start, const int end) const {
	for (int i = start + 1; i < end; ++i) {
		for (int j = 0; j < environment->particleCount; ++j)
			if (i != j)
				ParticleCollision(i, j);
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

void PhysicsEngine::QuadTreeParticleCollisions(std::vector<QuadTree>& quads, const int start, const int end) const {
	for (int i = start; i < end; ++i) {
		QuadTreeParticleCollisions(quads[i]);
	}
}

void PhysicsEngine::UpdateParticles(int start, int end, float deltaTime) const {
	for (int i = start; i < end; i++) {
		environment->particleVel[i] += gravity * deltaTime;
		environment->particlePos[i] += environment->particleVel[i] * deltaTime;
		BoundingBoxCollision(i);
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
	Timer timer2;

	std::vector<Range> particleSections;
	environment->workerThreads.PartitionForWorkers(environment->particleCount, particleSections, 1);

	float explosionTime = 0, updateTime = 0, quadTreeTime = 0, particleCollisionTime = 0;

	timer2.Start();
	while (!environment->done) {

		float deltaTime = timer.DeltaTime();
		stats->physicsTimeRatioTotalLastSecond += timer.RealTimeDifference();

		//EXPLOSIONS
		timer.Restart();
		HandleExplosions();
		timer.Stop();
		explosionTime += timer.ElapsedSeconds();

		//UPDATES
		timer.Restart();
		for (auto particleSection : particleSections)
			environment->workerThreads.AddWork([=] { UpdateParticles(particleSection.lower, particleSection.upper, deltaTime); });
		environment->workerThreads.JoinWorkerThreads();
		timer.Stop();
		updateTime += timer.ElapsedSeconds();

		//QUADTREE
		timer.Restart();
		environment->renderLock.lock();
		auto quads = environment->tree->Build(*stats);
		environment->renderLock.unlock();
		timer.Stop();
		quadTreeTime += timer.ElapsedSeconds();

		//PARTICLE COLLISIONS
		timer.Restart();
		std::vector<Range> quadSections;
		environment->workerThreads.PartitionForWorkers(quads.size(), quadSections, 1);
		for (auto quadSection : quadSections)
			environment->workerThreads.AddWork([=, &quads] { QuadTreeParticleCollisions(quads, quadSection.lower, quadSection.upper); });
		/*for (QuadTree quad : quads) {
			environment->workerThreads.AddWork([=] {QuadTreeParticleCollisions(quad); });
		}*/
		//for (int i = 0; i < environment->workerThreadCount + 1; ++i) {
		//	environment->workerThreads.AddWork([=] { ParticleCollisionsNonQuadTreee(particleSections[i].lower, particleSections[i].upper); });
		//}
		environment->workerThreads.JoinWorkerThreads();
		timer.Stop();
		particleCollisionTime += timer.ElapsedSeconds();

		++stats->physicsUpdateTotalLastSecond;

		if (timer2.ElapsedSeconds() >= 1) {			
			std::cout << "\n\nPhysics task time: \n\n";
			std::cout << "explosionTime: " << std::to_string(explosionTime) << "\n";
			std::cout << "updateTime: " << std::to_string(updateTime) << "\n";
			std::cout << "quadTreeTime: " << std::to_string(quadTreeTime) << "\n";
			std::cout << "particleCollisionTime: " << std::to_string(particleCollisionTime) << "\n\n\n";
			explosionTime = updateTime = quadTreeTime = particleCollisionTime = 0;
			timer2.Restart();
		}
	}

	environment->workerThreads.CloseWorkerThreads();
}
