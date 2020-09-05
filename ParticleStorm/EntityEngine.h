#pragma once
#include <vector>
#include "BaseEntity.h"
#include "MessageSystem.h"
#include "Configuration.h"

class EntityEngine {
public:
	EntityEngine(MessageSystem* messageSystem);
	~EntityEngine();
	
	void ApplyConfiguration(Configuration* config);
	void DestroyAllEntities();

	void Update();

	int GenerateUniqueId();
	void SubmitEntity(BaseEntity* entity);
	void DestroyEntity(BaseEntity* entity);


private:
	MessageSystem* messageSystem;

	int nextId;

	std::vector<BaseEntity*> entities;
	std::vector<BaseEntity*> destroyedEntities;


	void CreateAndSubmitParticles(Configuration* config);
	void CreateAndSubmitWorld(Configuration* config);
};

