#pragma once
#include "ParticlesEntity.h"
#include "SessionManager.h"
#include "DebugQuadTreeEntity.h"

class SandboxSession : SessionManager {
public:
	SandboxSession(MessageSystem* messageQueue, EventEngine* eventEngine, Camera* camera, Stats* stats);
	~SandboxSession();
	
	void Init() override;
	void Update() override;
	void Complete() override;
	
private:
	WorldEntity* world;
	ParticlesEntity* particles;
	DebugQuadTreeEntity* debugQuadTree;
	
	void HandleEntityDestroyed(BaseEntity* entity);
	void HandleMessages() override;
};

