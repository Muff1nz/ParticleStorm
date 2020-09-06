#include "CoreEngine.h"
#include <iostream>
#include <cctype>
#include "WorkerThreadPool.h"
#include "Timer.h"
#include "EntityEngine.h"
#include "SandboxSession.h"


CoreEngine::CoreEngine() = default;

CoreEngine::~CoreEngine() = default;


void CoreEngine::BootParticleStorm() {
	shouldRun = true;

	StartEngine();

	char userInput;
	do {
		PrintMenu();
		userInput = getchar();
		std::cin.ignore();
		switch (std::toupper(userInput)) {
		case '1':
			BootSandbox();
			break;
		case '2':
			//BootPhysBench();
			break;
		case '3':
			//BootGraphBench();
			break;
		case 'X':
			break;
		default:
			std::cout << "\nInvalid option!\n";
			break;
		}
	} while (std::toupper(userInput) != 'X' && shouldRun);

	StopEngine();
}

void CoreEngine::StartEngine() {
	messageSystem = new MessageSystem();
	workerThreadPool = new WorkerThreadPool(14);

	stats = new Stats();

	renderEngine = new RenderEngineVulkan(messageSystem, stats);
	physicsEngine = new  PhysicsEngine(messageSystem, workerThreadPool, stats);
	eventEngine = new EventEngine(messageSystem);
	entityEngine = new EntityEngine(messageSystem);

	renderEngine->Init();
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
			
	delete physicsEngine;
	delete renderEngine;
	delete eventEngine;
	delete entityEngine;
	delete stats;
	delete workerThreadPool;
	delete messageSystem;
}

void CoreEngine::PrintMenu() const {
	std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
	std::cout << "ParticleStorm Main Menu:\n";
	std::cout << "Sandbox(1):\n";
	std::cout << "Physics Benchmark(2) (Out of order)\n";
	std::cout << "Graphics Benchmark(3) (Out of order)\n";
	std::cout << "Quit(X)\n";
}


void CoreEngine::BootSandbox() {
	auto camera = renderEngine->GetCamera();

	bool shouldRunSession = true;

	Timer timer;
	timer.Start();

	SandboxSession sandboxSession(messageSystem, eventEngine, camera, stats);
	sandboxSession.Init();

	while (shouldRunSession && shouldRun) {
		auto deltaTime = timer.DeltaTime();

		eventEngine->Update();
		entityEngine->Update();
		camera->Update(deltaTime);
		sandboxSession.Update();

		Message message = messageSystem->PS_GetMessage(SYSTEM_CoreEngine);
		while (!message.IsEmpty()) {
			switch (message.messageType) {
			case MT_ShutDown:
				shouldRun = false;
				break;
			case MT_Shutdown_Session:
				shouldRunSession = false;
				break;
			default:
				break;
			}
			message = messageSystem->PS_GetMessage(SYSTEM_CoreEngine);
		}

		std::this_thread::sleep_for(std::chrono::microseconds(20));
	}

	entityEngine->DestroyAllEntities();
	while (!entityEngine->AllEntitiesAreDead()) {
		entityEngine->Update();
	}
	
	sandboxSession.Complete();
}

void CoreEngine::BootPhysBench() {
	//auto camera = renderEngine->GetCamera();

	//bool shouldRunSession = true;

	//Timer timer;
	//timer.Start();

	//SandboxSession SandboxSession(messageSystem, eventEngine, camera, stats);
	//SandboxSession.Init();

	//while (shouldRunSession && shouldRun) {
	//	auto deltaTime = timer.DeltaTime();

	//	eventEngine->Update();
	//	entityEngine->Update();
	//	camera->Update(deltaTime);
	//	SandboxSession.Update();

	//	Message message = messageSystem->PS_GetMessage(SYSTEM_CoreEngine);
	//	while (!message.IsEmpty()) {
	//		switch (message.messageType) {
	//		case MT_ShutDown:
	//			shouldRun = false;
	//			break;
	//		case MT_Shutdown_Session:
	//			shouldRunSession = false;
	//			break;
	//		case MT_Config:
	//			entityEngine->ApplyConfiguration(static_cast<Configuration*>(message.payload));
	//			break;
	//		default:
	//			break;
	//		}
	//		message = messageSystem->PS_GetMessage(SYSTEM_CoreEngine);
	//	}

	//	std::this_thread::sleep_for(std::chrono::microseconds(20));
	//}

	//entityEngine->DestroyAllEntities();
	//SandboxSession.CompleteSandbox();
}

void CoreEngine::BootGraphBench() {

}