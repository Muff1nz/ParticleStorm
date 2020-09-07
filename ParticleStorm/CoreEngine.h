#pragma once
#include "PhysicsEngine.h"
#include "RenderEngineVulkan.h"
#include "SessionManager.h"
#include "EntityEngine.h"

class CoreEngine {
public:
	CoreEngine();
	~CoreEngine();

	void BootParticleStorm();
private:
	bool shouldRun;

	MessageSystem* messageSystem{};
	WorkerThreadPool* workerThreadPool{};
	Stats* stats{};
	
	PhysicsEngine* physicsEngine{};
	RenderEngineVulkan* renderEngine{};
	EventEngine* eventEngine{};
	EntityEngine* entityEngine{};

	void StartEngine();
	void StopEngine();

	void PrintMenu() const;
	void BootSession(SessionManager& sessionManager);
};

