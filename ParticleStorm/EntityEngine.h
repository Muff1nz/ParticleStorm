#pragma once
#include <vector>
#include "BaseEntity.h"
#include "MessageSystem.h"

class EntityEngine {
public:
	EntityEngine(MessageSystem* messageSystem);
	~EntityEngine();
		
	void DestroyAllEntities();

	void Update();

	void SubmitEntity(BaseEntity* entity);
	void DestroyEntity(BaseEntity* entity);

	bool AllEntitiesAreDead() const;

private:
	MessageSystem* messageSystem;

	int nextId;

	std::vector<BaseEntity*> entities;
	std::vector<BaseEntity*> destroyedEntities;

	void HandleMessages();
	void DestroyDeadObjects();
};

