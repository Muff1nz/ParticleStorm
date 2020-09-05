#include "PhysicsEngine.h"

#include <detail/func_geometric.inl>

#include "Timer.h"
#include "Range.h"
#include "LinearQuad.h"
#include "QuadTreeHandler.h"
#include "ParticlesEntity.h"
#include "WorldEntity.h"

PhysicsEngine::PhysicsEngine(MessageSystem* messageQueue, WorkerThreadPool* workerThreads, Stats* stats) {
	this->messageQueue = messageQueue;
	this->workerThreads = workerThreads;
	this->stats = stats;

	collisionChecker = CollisionChecker(1);
	quadTreeHandler = new QuadTreeHandler(workerThreads, stats);
}

PhysicsEngine::~PhysicsEngine() = default;

void PhysicsEngine::Join() {
	LeadPhysicsThread.join();
}

void PhysicsEngine::Start() {
	LeadPhysicsThread = std::thread([=] {LeadThreadRun(); });
}

void PhysicsEngine::WorldBoundsCheck(const int particle) const {
	const auto circlePos = particles->position;
	const auto circleVel = particles->velocity;

	if (circlePos[particle].y < particles->radius) {
		circleVel[particle].y = -circleVel[particle].y * friction;
		circlePos[particle].y = particles->radius;
		particles->particleResting[particle] = true;
	}
	if (circlePos[particle].y > world->height - particles->radius) {
		circleVel[particle].y = -circleVel[particle].y * friction;
		circlePos[particle].y = world->height - particles->radius;
	}
	if (circlePos[particle].x < particles->radius) {
		circleVel[particle].x = -circleVel[particle].x * friction;
		circlePos[particle].x = particles->radius;
	}
	if (circlePos[particle].x > world->width - particles->radius) {
		circleVel[particle].x = -circleVel[particle].x * friction;
		circlePos[particle].x = world->width - particles->radius;
	}
}

void PhysicsEngine::ResolveCollision(const int particle1, const int particle2, const float dist) const {
	const auto particlePos = particles->position;
	const auto particleVel = particles->velocity;
	const auto particleResting = particles->particleResting;

	glm::vec2 positionDelta = particlePos[particle1] - particlePos[particle2];
	const float overlap = particles->radius * 2 - dist;
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
		tree->internalParticlePos[localParticle1] = particles->position[globalParticle1];
		tree->internalParticlePos[localParticle2] = particles->position[globalParticle2];
	}
}

void PhysicsEngine::QuadMixedParticleCollision(const int localParticle1, const int localParticle2, LinearQuad* tree) const {
	const int globalParticle2 = tree->externalParticle[localParticle2];
	float dist = 0;
	if (collisionChecker.CircleCircleCollision(tree->internalParticlePos[localParticle1], particles->position[globalParticle2], dist)) {
		const int globalParticle1 = tree->internalParticle[localParticle1];
		ResolveCollision(globalParticle1, globalParticle2, dist);
		tree->internalParticlePos[localParticle1] = particles->position[globalParticle1];
	}
}


void PhysicsEngine::QuadExternalCollision(const int localParticle1, const int localParticle2, LinearQuad* tree) const {
	const int globalParticle1 = tree->externalParticle[localParticle1];
	const int globalParticle2 = tree->externalParticle[localParticle2];
	float dist = 0;
	if (collisionChecker.CircleCircleCollision(particles->position[globalParticle1], particles->position[globalParticle2], dist)) {
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
		if (!particles->particleResting[i])
			particles->velocity[i] += gravity * deltaTime;
		particles->position[i] += particles->velocity[i] * deltaTime;

		particles->particleResting[i] = false;
		particles->particleQuadCount[i] = 0;
	}
}

void PhysicsEngine::HandleExplosion(Message message) const {
	if (particles == nullptr || world == nullptr)
		return;

	float const explosionForce = 500000.0f;
	const auto circlePos = particles->position;
	const auto circleVel = particles->velocity;

	glm::vec2 impactPoint = *static_cast<glm::vec2*>(message.payload);
	for (int i = 0; i < particles->count; i++) {
		glm::vec2 lineBetween = circlePos[i] - impactPoint;
		const float distance = length(lineBetween);
		lineBetween /= distance;
		circleVel[i] += lineBetween / distance * explosionForce;
	}
	delete static_cast<glm::vec2*>(message.payload);
}

void PhysicsEngine::AddEntity(Message message) {
	BaseEntity* entity = static_cast<BaseEntity*>(message.payload);

	if (entity->type == ET_Particles) {
		ParticlesEntity* particlesEntity = static_cast<ParticlesEntity*>(entity);

		if (particles != nullptr)
			throw std::runtime_error("Cannot add particles to physics engine, particles already present");

		particlesEntity->RegisterAsObserver();
		particles = new PhysicsParticlesEntity(particlesEntity);

		collisionChecker = CollisionChecker(particles->radius);
		workerThreads->PartitionForWorkers(particles->count, particleSections);
	}
	
	if (entity->type == ET_World) {
		WorldEntity* worldEntity = static_cast<WorldEntity*>(entity);

		if (world != nullptr)
			throw std::runtime_error("Cannot add world to physics engine, world already present");

		worldEntity->RegisterAsObserver();
		world = worldEntity;
	}

	if (world != nullptr && particles != nullptr) {
		quadTreeHandler->Init(particles, world);
	}
}

void PhysicsEngine::RemoveEntity(Message message) {
	BaseEntity* entity = static_cast<BaseEntity*>(message.payload);

	if (entity->type == ET_Particles) {
		ParticlesEntity* particlesEntity = static_cast<ParticlesEntity*>(message.payload);
		if (particles != nullptr && particles->particlesEntity->id != particlesEntity->id)
			throw std::runtime_error("Cannot remove particles from physics engine! Existing particles don't match particles to remove!");

		particles->Dispose();
		particles->particlesEntity->UnregisterAsObserver();
		particles = nullptr;
	}


	if (entity->type == ET_World) {
		WorldEntity* worldEntity = static_cast<WorldEntity*>(message.payload);
		if (world != nullptr && world->id != worldEntity->id)
			throw std::runtime_error("Cannot remove world from physics engine! Existing world don't match world to remove!");

		worldEntity->UnregisterAsObserver();
		world = nullptr;
	}
}

void PhysicsEngine::HandleMessages() {
	Message message = messageQueue->PS_GetMessage(SYSTEM_PhysicsEngine);
	while (!message.IsEmpty()) {
		switch (message.messageType) {
		case MT_ShutDown:
			shouldRun = false;
			break;
		case MT_Explosion:
			HandleExplosion(message);
			break;
		case MT_DebugModeToggle:
			debugMode = !debugMode;
			break;
		case MT_Entity_Submitted:
			AddEntity(message);
			break;
		case MT_Entity_Destroyed:
			RemoveEntity(message);
		default: 
			break;
		}
		message = messageQueue->PS_GetMessage(SYSTEM_PhysicsEngine);
	}
}


void PhysicsEngine::LeadThreadRun() {

	Timer timer(maxPhysicsDeltaTime, minPhysicsDeltaTime);
	
	auto linearQuads = new std::vector<LinearQuad*>();

	shouldRun = true;
	while (shouldRun) {

		float deltaTime = timer.DeltaTime();
		stats->physicsTimeRatioTotalLastSecond += timer.RealTimeDifference();

		//Messages
		timer.Restart();
		HandleMessages();
		timer.Stop();
		stats->puEventsTotalLastSecond += timer.ElapsedMicroseconds();

		if (particles == nullptr || world == nullptr) {
			std::this_thread::sleep_for(std::chrono::microseconds(100));
			continue;
		}

		//QUADTREE
		timer.Restart();
		std::vector<Range> quadSections;
		quadTreeHandler->BuildLinearQuadTree(linearQuads, quadSections);
		timer.Stop();
		stats->puQuadTreeUpdateTotalLastSecond += timer.ElapsedMicroseconds();

		if (debugMode)
			quadTreeHandler->PopulateQuadData();

		//PARTICLE COLLISIONS
		timer.Restart();
		for (auto quadSection : quadSections)
			workerThreads->AddWork([=] { LinearQuadParticleCollisions(linearQuads, quadSection.lower, quadSection.upper); });
		workerThreads->JoinWorkerThreads();
		timer.Stop();
		stats->puCollisionUpdateTotalLastSecond += timer.ElapsedMicroseconds();

		//UPDATES
		timer.Restart();
		for (auto particleSection : particleSections)
			workerThreads->AddWork([=] { UpdateParticles(particleSection.lower, particleSection.upper, deltaTime); });
		workerThreads->JoinWorkerThreads();
		timer.Stop();
		stats->puPositionUpdatesTotalLastSecond += timer.ElapsedMicroseconds();

		++stats->physicsUpdateTotalLastSecond;
	}

	for (int i = 0; i < linearQuads->size(); i++)
		delete (*linearQuads)[i];

	delete linearQuads;
}
