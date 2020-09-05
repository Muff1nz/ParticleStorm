#include "EntityEngine.h"
#include "WorldEntity.h"
#include "NumberGenerator.h"
#include "ParticlesEntity.h"


EntityEngine::EntityEngine(MessageSystem* messageSystem) {
	this->messageSystem = messageSystem;
}


EntityEngine::~EntityEngine() {
	for (BaseEntity* registeredEntity : destroyedEntities) {
		delete registeredEntity;
	}

	for (BaseEntity* registeredEntity : entities) {
		delete registeredEntity;
	}
}

void EntityEngine::DestroyAllEntities() {
	for (BaseEntity* registeredEntity : entities) {
		DestroyEntity(registeredEntity);
	}
}

void EntityEngine::ApplyConfiguration(Configuration* config) {
	DestroyAllEntities();

	CreateAndSubmitWorld(config);
	CreateAndSubmitParticles(config);	
}

void EntityEngine::Update() {
	for (int i = 0; i < destroyedEntities.size(); ++i) {
		BaseEntity* registeredEntity = destroyedEntities[i];
		if (registeredEntity->GetObservers() == 0) {
			destroyedEntities.erase(destroyedEntities.begin() + i);
			i--;
			delete registeredEntity;
		}
	}
}

int EntityEngine::GenerateUniqueId() {
	return nextId++;
}

void EntityEngine::SubmitEntity(BaseEntity* entity) {
	for (BaseEntity* registeredEntity : entities) {
		if (registeredEntity->id == entity->id)
			throw std::runtime_error("Entity already submitted!");
	}

	entities.emplace_back(entity);
	messageSystem->PS_BroadcastMessage(Message(SYSTEM_EntityEngine, MT_Entity_Submitted, entity));
}

void EntityEngine::DestroyEntity(BaseEntity* entity) {
	for (int i = 0; i < entities.size(); ++i) {
		BaseEntity* registeredEntity = entities[i];
		if (registeredEntity->id == entity->id) {
			entities.erase(entities.begin() + i);
			destroyedEntities.emplace_back(registeredEntity);

			messageSystem->PS_BroadcastMessage(Message(SYSTEM_EntityEngine, MT_Entity_Destroyed, entity));
			return;
		}
	}

	throw std::runtime_error("Can't destroy entity that is not submitted!");
}

void EntityEngine::CreateAndSubmitParticles(Configuration* config) {
	auto particles = new ParticlesEntity();
	particles->id = GenerateUniqueId();
	particles->count = config->particleCount;
	particles->radius = config->particleRadius;
	particles->position = new glm::vec2[particles->count];
	particles->velocity = new glm::vec2[particles->count];
	particles->scale = new glm::vec2(particles->radius, particles->radius);
	particles->texturePath = "Textures/Circle.png";

	auto rng = NumberGenerator(config->seed);

	const int maxSpeed = 1000;
	const auto circlePos = particles->position;
	const auto circleVel = particles->velocity;

	for (int i = 0; i < particles->count; i++) { //Initialize particles (position/velocity)
		circlePos[i] = glm::vec2(rng.GenerateFloat(0, config->worldWidth), rng.GenerateFloat(0, config->worldHeight));

		do {
			circleVel[i] = glm::vec2(rng.GenerateFloat(-maxSpeed, maxSpeed), rng.GenerateFloat(-maxSpeed, maxSpeed));
		} while (abs(circleVel[i].x) < 1 && abs(circleVel[i].y) < 1);
	}

	SubmitEntity(particles);
}

void EntityEngine::CreateAndSubmitWorld(Configuration* config) {
	auto world = new WorldEntity();
	world->id = GenerateUniqueId();
	world->width = config->worldWidth;
	world->height = config->worldHeight;
	world->position = new glm::vec2(world->width / 2, world->height / 2);
	world->scale = new glm::vec2(world->width / 2, world->height / 2);
	world->texturePath = "Textures/BackGround.png";

	SubmitEntity(world);
}
