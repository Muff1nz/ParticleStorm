#include "CoreEngine.h"
#include <iostream>
#include <cctype>
#include "WorkerThreadPool.h"
#include "Timer.h"
#include "EntityEngine.h"

//New design philosophy:
//Game logic happens in SessionManager (It decides how big the world is and how many particles there are in it)
//Environment is kind of replaced by SessionManager and the EntityEngine system. SessionManager uses the EntityEngine to create game objects. 
//Which then defines everything else (Like world size and so on)



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
			BootPhysBench();
			break;
		case '3':
			BootGraphBench();
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
	workerThreadPool = new WorkerThreadPool(14);

	stats = new Stats();

	renderEngine = new RenderEngineVulkan(&messageSystem, stats);
	physicsEngine = new  PhysicsEngine(&messageSystem, workerThreadPool, stats);
	eventEngine = new EventEngine(&messageSystem);
	entityEngine = new EntityEngine(&messageSystem);

	renderEngine->Init();
	physicsEngine->Init();
	eventEngine->Init(renderEngine->GetWindow());

	auto camera = renderEngine->GetCamera();
	camera->SetEventEngine(eventEngine);

	renderEngine->Start();
	physicsEngine->Start();

	sessionManager = new SessionManager(&messageSystem, eventEngine, entityEngine, camera, stats);
}

void CoreEngine::StopEngine() {
	renderEngine->Join();
	physicsEngine->Join();
	workerThreadPool->CloseWorkerThreads();

	renderEngine->Dispose();

	delete workerThreadPool;
	delete stats;
	delete sessionManager;
	delete physicsEngine;
	delete renderEngine;
	delete eventEngine;
	delete entityEngine;
}

void CoreEngine::PrintMenu() const {
	std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
	std::cout << "ParticleStorm Main Menu:\n";
	std::cout << "Sandbox(1):\n";
	std::cout << "Physics Benchmark(2)\n";
	std::cout << "Graphics Benchmark(3)\n";
	std::cout << "Quit(X)\n";
}


void CoreEngine::BootSandbox() {
	auto camera = renderEngine->GetCamera();


	bool shouldRunSession = true;

	Timer timer;
	timer.Start();

	sessionManager->InitSandbox();
	while (shouldRunSession && shouldRun) {
		auto deltaTime = timer.DeltaTime();

		eventEngine->Update();
		entityEngine->Update();
		camera->Update(deltaTime);
		sessionManager->SandboxUpdate();

		Message message = messageSystem.PS_GetMessage(SYSTEM_CoreEngine);
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
			message = messageSystem.PS_GetMessage(SYSTEM_CoreEngine);
		}

		std::this_thread::sleep_for(std::chrono::microseconds(20));
	}

	sessionManager->CompleteSandbox();


}

void CoreEngine::BootPhysBench() {
	
}

void CoreEngine::BootGraphBench() {

}