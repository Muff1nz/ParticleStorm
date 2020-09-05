#pragma once
#include "SessionManager.h"

class SandboxSession : SessionManager {
public:
	SandboxSession(MessageSystem* messageQueue, EventEngine* eventEngine, Camera* camera, Stats* stats);
	~SandboxSession();
	
	void Init() override;
	void Update() override;
	void Complete() override;
	
private:
	void HandleMessages() override;
};

