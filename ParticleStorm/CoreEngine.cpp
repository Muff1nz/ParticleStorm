#include "CoreEngine.h"
#include <iostream>
#include "WorkerThreadPool.h"
#include "Timer.h"
#include "EntityEngine.h"
#include "GraphicsBenchmarkSession.h"
#include "MainMenuSession.h"
#include "PhysicsBenchmarkSession.h"
#include "SandboxSession.h"


CoreEngine::CoreEngine() = default;

CoreEngine::~CoreEngine() = default;


void CoreEngine::BootParticleStorm() {
	StartEngine();

	SessionResult result;
	do {
		result = BootSession(MainMenuSession(messageSystem, eventEngine, renderEngine->GetCamera(), stats));
			
		switch (result) {
		case SR_START_SANDBOX:
			BootSession(SandboxSession(messageSystem, eventEngine, renderEngine->GetCamera(), stats));
			break;
		case SR_START_PHYSICS_BENCHMARK:
			BootSession(PhysicsBenchmarkSession(messageSystem, eventEngine, renderEngine->GetCamera(), stats));
			break;
		case SR_START_GRAPHICS_BENCHMARK:
			BootSession(GraphicsBenchmarkSession(messageSystem, eventEngine, renderEngine->GetCamera(), stats));
			break;
		default:
			break;
		}
	} while (result != SR_EXIT_GAME);

	StopEngine();
}

void CoreEngine::StartEngine() {
	messageSystem = new MessageSystem();
	workerThreadPool = new WorkerThreadPool(14);

	stats = new Stats();

	eventEngine = new EventEngine(messageSystem);
	entityEngine = new EntityEngine(messageSystem);
	
	renderEngine = new RenderEngineVulkan(messageSystem, stats);
	guiEngine = new GuiEngine(eventEngine, messageSystem);
	physicsEngine = new  PhysicsEngine(messageSystem, workerThreadPool, stats);

	renderEngine->Init();
	guiEngine->Init(renderEngine->GetCamera());
	eventEngine->Init(renderEngine->GetWindow());
	
	auto camera = renderEngine->GetCamera();
	camera->SetEventEngine(eventEngine);

	renderEngine->Start();
	physicsEngine->Start();
}

void CoreEngine::StopEngine() {
	messageSystem->PS_BroadcastMessage(Message(SYSTEM_CoreEngine, MT_ShutDown));
	
	renderEngine->Join();
	physicsEngine->Join();
	workerThreadPool->CloseWorkerThreads();

	renderEngine->Dispose();
			
	delete eventEngine;
	delete entityEngine;

	delete renderEngine;
	delete guiEngine;
	delete physicsEngine;

	delete stats;	
	delete workerThreadPool;
	delete messageSystem;
}

SessionResult CoreEngine::BootSession(SessionManager& sessionManager) const {
	auto camera = renderEngine->GetCamera();

	bool shouldRunSession = true;

	Timer timer;
	timer.Start();

	sessionManager.Init();

	while (shouldRunSession) {
		auto deltaTime = timer.DeltaTime();

		Message message = messageSystem->PS_GetMessage(SYSTEM_CoreEngine);
		while (!message.IsEmpty()) {
			switch (message.messageType) {
			case MT_ShutDown:
				return SR_EXIT_GAME;
			case MT_Shutdown_Session:
				shouldRunSession = false;
				break;
			case MT_Config:
			{
				auto config = static_cast<Configuration*>(message.payload);
				if (config->workerThreadCount != workerThreadPool->GetThreadCount()) {
					workerThreadPool->SetThreadCount(config->workerThreadCount);
				}
			}
			break;
			default:
				break;
			}
			message = messageSystem->PS_GetMessage(SYSTEM_CoreEngine);
		}

		eventEngine->Update();
		entityEngine->Update();
		guiEngine->Update();
		camera->Update(deltaTime);
		sessionManager.Update();
		
		std::this_thread::sleep_for(std::chrono::microseconds(20));
	}

	entityEngine->DestroyAllEntities();
	while (!entityEngine->AllEntitiesAreDead()) {
		sessionManager.Update();
		entityEngine->Update();
		guiEngine->Update();
	}

	return sessionManager.Complete();
}
