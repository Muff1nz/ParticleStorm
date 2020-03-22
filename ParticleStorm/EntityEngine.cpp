#include "EntityEngine.h"



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
