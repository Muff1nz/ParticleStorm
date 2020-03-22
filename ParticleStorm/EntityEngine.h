#pragma once
#include <vector>
#include "BaseEntity.h"
#include "MessageSystem.h"

class EntityEngine {
public:
	EntityEngine(MessageSystem* messageSystem);
	~EntityEngine();

	void Update();

	int GenerateUniqueId();
	void SubmitEntity(BaseEntity* entity);
	void DestroyEntity(BaseEntity* entity);
private:
	MessageSystem* messageSystem;

	int nextId;

	std::vector<BaseEntity*> entities;
	std::vector<BaseEntity*> destroyedEntities;
};

