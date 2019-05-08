#include "PhysicsEngine.h"
#include <iostream>
#include "Timer.h"
#include "Range.h"
#include <map>
#include "QuadTree.h"
#include <detail/func_geometric.inl>

PhysicsEngine::PhysicsEngine(Environment* environment) {
	this->environment = environment;
	collisionChecker = CollisionChecker(environment->particleRadius);
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
}

void PhysicsEngine::Join() {
	LeadThread.join();
}

void PhysicsEngine::Start() {
	LeadThread = std::thread([=] {LeadThreadRun(); });
}

void PhysicsEngine::WorldBoundsCheck(const int particle) const {
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

void PhysicsEngine::QuadInternalParticleCollision(const int localParticle1, const int localParticle2, QuadTree* tree) const {
	float dist = 0;
	if (collisionChecker.CircleCircleCollision(tree->internalParticlePos[localParticle1], tree->internalParticlePos[localParticle2], dist)) {
		const int globalParticle1 = tree->internalParticle[localParticle1];
		const int globalParticle2 = tree->internalParticle[localParticle2];
		ResolveCollision(globalParticle1, globalParticle2, dist);
		tree->internalParticlePos[localParticle1] = environment->particlePos[globalParticle1];
		tree->internalParticlePos[localParticle2] = environment->particlePos[globalParticle2];
	}
}

void PhysicsEngine::QuadMixedParticleCollision(const int localParticle1, const int localParticle2, QuadTree* tree) const {
	const int globalParticle2 = tree->externalParticle[localParticle2];
	float dist = 0;
	if (collisionChecker.CircleCircleCollision(tree->internalParticlePos[localParticle1], environment->particlePos[globalParticle2], dist)) {
		const int globalParticle1 = tree->internalParticle[localParticle1];
		ResolveCollision(globalParticle1, globalParticle2, dist);
		tree->internalParticlePos[localParticle1] = environment->particlePos[globalParticle1];
	}
}


void PhysicsEngine::QuadExternalCollision(const int localParticle1, const int localParticle2, QuadTree* tree) const {
	const int globalParticle1 = tree->externalParticle[localParticle1];
	const int globalParticle2 = tree->externalParticle[localParticle2];
	float dist = 0;
	if (collisionChecker.CircleCircleCollision(environment->particlePos[globalParticle1], environment->particlePos[globalParticle2], dist)) {
		ResolveCollision(globalParticle1, globalParticle2, dist);
	}
}

void PhysicsEngine::QuadTreeParticleCollisions(QuadTree* tree) const {
	const int internalParticles = tree->internalParticle.size();
	const int externalParticles = tree->externalParticle.size();

	for (int i = 0; i < internalParticles; ++i) {	//All internal particles against each other
		for (int j = i + 1; j < internalParticles; ++j) {
			QuadInternalParticleCollision(i, j, tree);
		}
	}

	for (int i = 0; i < internalParticles; ++i) {	//All internal particles against all external particles
		for (int j = 0; j < externalParticles; ++j) {
			QuadMixedParticleCollision(i, j, tree);
		}
	}

	for (int i = 0; i < externalParticles; ++i) {		//All external particles against each other
		for (int j = i + 1; j < externalParticles; ++j) {
			QuadExternalCollision(i, j, tree);
		}
	}
}

void PhysicsEngine::QuadTreeParticleCollisions(ConcurrentVector<QuadTree*>* quads, const int start, const int end) const {
	for (int i = start; i < end; ++i) {
		QuadTreeParticleCollisions((*quads)[i]);
	}
}

void PhysicsEngine::CalculateQuadTreeOverflow(ConcurrentVector<QuadTree*>* quads, const int start, const int end) const {
	for (int i = start; i < end; ++i) {
		QuadTree* quad = (*quads)[i];
		quad->externalParticle.clear();
		quad->internalParticle.clear();
		quad->internalParticlePos.clear();
		for (int j = 0; j < quad->particlesInQuad.size(); ++j) {
			const int globalParticle = quad->particlesInQuad[j];
			if (environment->particleQuadCount[globalParticle] > 1) {
				quad->externalParticle.push_back(globalParticle);
			} else {
				quad->internalParticle.push_back(globalParticle);
				quad->internalParticlePos.push_back(environment->particlePos[globalParticle]);
			}
		}
	}
}

void PhysicsEngine::UpdateParticles(int start, int end, float deltaTime) const {
	for (int i = start; i < end; i++) {
		WorldBoundsCheck(i);
		if (!environment->particleResting[i])
			environment->particleVel[i] += gravity * deltaTime;
		environment->particlePos[i] += environment->particleVel[i] * deltaTime;

		environment->particleResting[i] = false;
		environment->particleQuadCount[i] = 0;
	}
}

void PhysicsEngine::HandleExplosions() const {
	float const explosionForce = 500000.0f;
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


	ConcurrentVector<QuadTree*>* quads = new ConcurrentVector<QuadTree*>();
	QuadTree tree(nullptr, environment, Rect(0, 0, environment->worldWidth, environment->worldHeight), quads);

	while (!environment->done) {

		float deltaTime = timer.DeltaTime();
		environment->stats.physicsTimeRatioTotalLastSecond += timer.RealTimeDifference();

		//EXPLOSIONS
		timer.Restart();
		HandleExplosions();
		timer.Stop();
		environment->stats.puEventsTotalLastSecond += timer.ElapsedMicroseconds();

		//QUADTREE
		timer.Restart();
		tree.BuildRoot();
		environment->workerThreads.JoinWorkerThreads();
		std::vector<Range> quadSections;
		environment->workerThreads.PartitionForWorkers(quads->size(), quadSections);
		for (auto quadSection : quadSections)
			environment->workerThreads.AddWork([=] { CalculateQuadTreeOverflow(quads, quadSection.lower, quadSection.upper); });
		environment->workerThreads.JoinWorkerThreads();
		timer.Stop();
		environment->stats.puQuadTreeUpdateTotalLastSecond += timer.ElapsedMicroseconds();

		//PARTICLE COLLISIONS
		timer.Restart();
		for (auto quadSection : quadSections)
			environment->workerThreads.AddWork([=] { QuadTreeParticleCollisions(quads, quadSection.lower, quadSection.upper); });
		environment->workerThreads.JoinWorkerThreads();
		timer.Stop();
		environment->stats.puCollisionUpdateTotalLastSecond += timer.ElapsedMicroseconds();

		//UPDATES
		timer.Restart();
		for (auto particleSection : particleSections)
			environment->workerThreads.AddWork([=] { UpdateParticles(particleSection.lower, particleSection.upper, deltaTime); });
		environment->workerThreads.JoinWorkerThreads();
		timer.Stop();
		environment->stats.puPositionUpdatesTotalLastSecond += timer.ElapsedMicroseconds();

		++environment->stats.physicsUpdateTotalLastSecond;
	}

	std::cout << "\nJOINING THE THREADS!!\n";
	environment->workerThreads.CloseWorkerThreads();
	std::cout << "\nRUN COMPLETE!!\n";
}
