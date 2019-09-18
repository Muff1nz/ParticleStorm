#include "PhysicsEngine.h"

#include <detail/func_geometric.inl>

#include "Timer.h"
#include "Range.h"
#include "LinearQuad.h"
#include "QuadTreeHandler.h"
#include "ConstStrings.h"

PhysicsEngine::PhysicsEngine(Environment* environment, MessageQueue* messageQueue) {
	this->environment = environment;
	this->messageQueue = messageQueue;
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

void PhysicsEngine::QuadInternalParticleCollision(const int localParticle1, const int localParticle2, LinearQuad* tree) const {
	float dist = 0;
	if (collisionChecker.CircleCircleCollision(tree->internalParticlePos[localParticle1], tree->internalParticlePos[localParticle2], dist)) {
		const int globalParticle1 = tree->internalParticle[localParticle1];
		const int globalParticle2 = tree->internalParticle[localParticle2];
		ResolveCollision(globalParticle1, globalParticle2, dist);
		tree->internalParticlePos[localParticle1] = environment->particlePos[globalParticle1];
		tree->internalParticlePos[localParticle2] = environment->particlePos[globalParticle2];
	}
}

void PhysicsEngine::QuadMixedParticleCollision(const int localParticle1, const int localParticle2, LinearQuad* tree) const {
	const int globalParticle2 = tree->externalParticle[localParticle2];
	float dist = 0;
	if (collisionChecker.CircleCircleCollision(tree->internalParticlePos[localParticle1], environment->particlePos[globalParticle2], dist)) {
		const int globalParticle1 = tree->internalParticle[localParticle1];
		ResolveCollision(globalParticle1, globalParticle2, dist);
		tree->internalParticlePos[localParticle1] = environment->particlePos[globalParticle1];
	}
}


void PhysicsEngine::QuadExternalCollision(const int localParticle1, const int localParticle2, LinearQuad* tree) const {
	const int globalParticle1 = tree->externalParticle[localParticle1];
	const int globalParticle2 = tree->externalParticle[localParticle2];
	float dist = 0;
	if (collisionChecker.CircleCircleCollision(environment->particlePos[globalParticle1], environment->particlePos[globalParticle2], dist)) {
		ResolveCollision(globalParticle1, globalParticle2, dist);
	}
}

void PhysicsEngine::LinearQuadParticleCollisions(LinearQuad* tree) const {
	const int internalParticles = tree->internalParticle.size();
	const int externalParticles = tree->externalParticle.size();
	
	for (int i = 0; i < externalParticles; ++i) {		//All external particles against each other
		for (int j = i + 1; j < externalParticles; ++j) {
			QuadExternalCollision(i, j, tree);
		}
	}

	for (int i = 0; i < internalParticles; ++i) {	//All internal particles against all external particles
		for (int j = 0; j < externalParticles; ++j) {
			QuadMixedParticleCollision(i, j, tree);
		}
	}

	for (int i = 0; i < internalParticles; ++i) {	//All internal particles against each other
		for (int j = i + 1; j < internalParticles; ++j) {
			QuadInternalParticleCollision(i, j, tree);
		}
	}
}

void PhysicsEngine::LinearQuadParticleCollisions(std::vector<LinearQuad*>* quads, const int start, const int end) const {
	for (int i = start; i < end; ++i) {
		LinearQuadParticleCollisions((*quads)[i]);
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

void PhysicsEngine::HandleExplosion(Message message) const {
	float const explosionForce = 500000.0f;
	const auto circlePos = environment->particlePos;
	const auto circleVel = environment->particleVel;

	glm::vec2 impactPoint = *static_cast<glm::vec2*>(message.payload);
	for (int i = 0; i < environment->particleCount; i++) {
		glm::vec2 lineBetween = circlePos[i] - impactPoint;
		const float distance = length(lineBetween);
		lineBetween /= distance;
		circleVel[i] += lineBetween / distance * explosionForce;
	}
	delete static_cast<glm::vec2*>(message.payload);
}

void PhysicsEngine::HandleMessages() {
	Message message = messageQueue->PS_GetMessage(SYSTEM_PhysicsEngine);
	while (!message.IsEmpty()) {
		switch (message.messageType) {
		case MT_Explosion:
			HandleExplosion(message);
			break;
		case MT_DebugModeToggle:
			debugMode = !debugMode;
			break;
		default: ;
		}
		message = messageQueue->PS_GetMessage(SYSTEM_PhysicsEngine);
	}
}


void PhysicsEngine::LeadThreadRun() {
	const auto circlePos = environment->particlePos;
	const auto circleVel = environment->particleVel;

	Timer timer(maxPhysicsDeltaTime, minPhysicsDeltaTime);

	std::vector<Range> particleSections;
	environment->workerThreads.PartitionForWorkers(environment->particleCount, particleSections);


	QuadTreeHandler quadTreeHandler(environment);
	auto linearQuads = new std::vector<LinearQuad*>();

	while (!environment->done) {

		float deltaTime = timer.DeltaTime();
		environment->stats.physicsTimeRatioTotalLastSecond += timer.RealTimeDifference();

		//EXPLOSIONS
		timer.Restart();
		HandleMessages();
		timer.Stop();
		environment->stats.puEventsTotalLastSecond += timer.ElapsedMicroseconds();

		//QUADTREE
		timer.Restart();
		std::vector<Range> quadSections;
		quadTreeHandler.BuildLinearQuadTree(linearQuads, quadSections);
		timer.Stop();
		environment->stats.puQuadTreeUpdateTotalLastSecond += timer.ElapsedMicroseconds();

		if (debugMode)
			quadTreeHandler.PopulateQuadData();

		//PARTICLE COLLISIONS
		timer.Restart();
		for (auto quadSection : quadSections)
			environment->workerThreads.AddWork([=] { LinearQuadParticleCollisions(linearQuads, quadSection.lower, quadSection.upper); });
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


	environment->workerThreads.CloseWorkerThreads();
}
