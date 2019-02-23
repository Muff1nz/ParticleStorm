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

	const auto dist = distance(particlePos[particle1], particlePos[particle2]);
	if (dist < doubleRadius) {
		glm::vec2 positionDelta = particlePos[particle1] - particlePos[particle2];

		const glm::vec2 newVel1 = particleVel[particle1] - dot(particleVel[particle1] - particleVel[particle2], positionDelta) / pow(length(positionDelta), 2) * positionDelta;

		positionDelta = -positionDelta;
		const glm::vec2 newVel2 = particleVel[particle2] - dot(particleVel[particle2] - particleVel[particle1], positionDelta) / pow(length(positionDelta), 2) * positionDelta;

		particleVel[particle1] = newVel1 * friction;
		particleVel[particle2] = newVel2 * friction;

		const float overlap = environment->particleRadius * 2 - dist;
		particlePos[particle1] += -(positionDelta / dist) * (overlap / 2);
		particlePos[particle2] += (positionDelta / dist) * (overlap / 2);
		++stats->particleCollisionTotalLastSecond;
	}
}

void PhysicsEngine::QuadTreeParticleCollisions(QuadTree* tree) const {
	for (int i = 0; i < tree->particlesInQuad.Size(); ++i) {
		for (int j = i + 1; j < tree->particlesInQuad.Size(); ++j) {
			ParticleCollision(tree->particlesInQuad[i], tree->particlesInQuad[j]);
		}
	}
}

void PhysicsEngine::QuadTreeParticleCollisions(ConcurrentVector<QuadTree*>* quads, const int start, const int end) const {
	for (int i = start; i < end; ++i) {
		QuadTreeParticleCollisions((*quads)[i]);
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

	std::vector<Range> particleSections;
	environment->workerThreads.PartitionForWorkers(environment->particleCount, particleSections, 1);

	ConcurrentVector<QuadTree*>* quads = new ConcurrentVector<QuadTree*>();

	while (!environment->done) {

		float deltaTime = timer.DeltaTime();
		stats->physicsTimeRatioTotalLastSecond += timer.RealTimeDifference();

		//EXPLOSIONS
		timer.Restart();
		HandleExplosions();
		timer.Stop();
		stats->puEventsTotalLastSecond += timer.ElapsedMilliseconds();

		//UPDATES
		timer.Restart();
		for (auto particleSection : particleSections)
			environment->workerThreads.AddWork([=] { UpdateParticles(particleSection.lower, particleSection.upper, deltaTime); });
		environment->workerThreads.JoinWorkerThreads();
		timer.Stop();
		stats->puPositionUpdatesTotalLastSecond += timer.ElapsedMilliseconds();

		//QUADTREE
		timer.Restart();
		environment->tree->BuildRoot(quads, stats);
		environment->workerThreads.JoinWorkerThreads();
		timer.Stop();
		stats->puQuadTreeUpdateTotalLastSecond += timer.ElapsedMilliseconds();

		//PARTICLE COLLISIONS
		timer.Restart();
		std::vector<Range> quadSections;
		environment->workerThreads.PartitionForWorkers(quads->Size(), quadSections, 1);
		for (auto quadSection : quadSections)
			environment->workerThreads.AddWork([=] { QuadTreeParticleCollisions(quads, quadSection.lower, quadSection.upper); });
		environment->workerThreads.JoinWorkerThreads();
		timer.Stop();
		stats->puCollisionUpdateTotalLastSecond += timer.ElapsedMilliseconds();

		++stats->physicsUpdateTotalLastSecond;
	}

	std::cout << "\nJOINING THE THREADS!!\n";
	environment->workerThreads.CloseWorkerThreads();
	std::cout << "\nRUN COMPLETE!!\n";
}
