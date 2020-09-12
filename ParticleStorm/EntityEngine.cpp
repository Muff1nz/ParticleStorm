#include "EntityEngine.h"

#include <iostream>

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

void EntityEngine::Update() {
	DestroyDeadObjects();
	HandleMessages();
}

bool EntityEngine::AllEntitiesAreDead() const {
	return entities.empty() && destroyedEntities.empty();
}

void EntityEngine::HandleMessages() {
	Message message = messageSystem->PS_GetMessage(SYSTEM_EntityEngine);
	while (!message.IsEmpty()) {
		switch (message.messageType) {
		case MT_Entity_Submit_Request:
			SubmitEntity(static_cast<BaseEntity*>(message.payload));
			break;
		case MT_Entity_Destroy_Request:
			DestroyEntity(static_cast<BaseEntity*>(message.payload));
			break;
		default:
			break;
		}
		message = messageSystem->PS_GetMessage(SYSTEM_EntityEngine);
	}
}

void EntityEngine::DestroyAllEntities() {
	for (BaseEntity* registeredEntity : entities) {
		destroyedEntities.emplace_back(registeredEntity);
		messageSystem->PS_BroadcastMessage(Message(SYSTEM_EntityEngine, MT_Entity_Destroyed, registeredEntity));
	}	
	entities.clear();
}

void EntityEngine::DestroyDeadObjects() {
	for (int i = 0; i < destroyedEntities.size(); ++i) {
		BaseEntity* registeredEntity = destroyedEntities[i];
		if (registeredEntity->GetObservers() == 0) {
			destroyedEntities.erase(destroyedEntities.begin() + i);
			i--;
			delete registeredEntity;
		}
	}
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
}