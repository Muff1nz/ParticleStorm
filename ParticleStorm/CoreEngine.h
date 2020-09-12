#pragma once
#include "PhysicsEngine.h"
#include "RenderEngineVulkan.h"
#include "SessionManager.h"
#include "EntityEngine.h"
#include "GuiEngine.h"

class CoreEngine {
public:
	CoreEngine();
	~CoreEngine();

	void BootParticleStorm();
private:

	MessageSystem* messageSystem{};
	WorkerThreadPool* workerThreadPool{};
	Stats* stats{};
	
	PhysicsEngine* physicsEngine{};
	RenderEngineVulkan* renderEngine{};
	EventEngine* eventEngine{};
	EntityEngine* entityEngine{};
	GuiEngine* guiEngine;

	void StartEngine();
	void StopEngine();

	void HandleMessages(bool& shouldRunSession);
	SessionResult BootSession(SessionManager& sessionManager) const;
};

