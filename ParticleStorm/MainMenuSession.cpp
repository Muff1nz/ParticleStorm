#include "MainMenuSession.h"

#include <iostream>

#include "ImageButtonEntity.h"

MainMenuSession::MainMenuSession(MessageSystem* messageQueue, EventEngine* eventEngine, Camera* camera, Stats* stats) : SessionManager(messageQueue, eventEngine, camera, stats) { }

MainMenuSession::~MainMenuSession() = default;

void MainMenuSession::Init() {
	camera->SetStatic(true);
	
	config.workerThreadCount = 2;
	config.screenHeight = 1200;
	config.screenWidth = 2800;
	messageQueue->PS_BroadcastMessage(Message(SYSTEM_SessionManager, MT_Config, static_cast<void*>(&config)));

	world = new WorldEntity(2064, 864);
	world->RegisterAsObserver();
	DeployEntity(world);
	
	std::string textures[4] = { "Textures/PlayButton.png" , "Textures/PhysBenchButton.png" , "Textures/GraphBenchButton.png" , "Textures/ExitButton.png" };
	std::function<void()> callbacks[4] = { [this] { OnPlayButtonClick(); } , [this] { OnPhysBenchButtonClick(); } , [this] { OnGraphBenchButtonClick(); } , [this] { OnExitButtonClick(); } };
	
	for (int i = 0; i < 4; ++i) {
		auto* ib = new ImageButtonEntity(textures[i], 200, 100, { 20, 864 - (i + 1) * 150 });
		ib->RegisterAsObserver();

		ib->RegisterOnClick(callbacks[i]);
		
		DeployEntity(ib);

		imageButtonEntities.push_back(ib);
	}
}

void MainMenuSession::Update() {
	if (eventEngine->GetKeyDown(GLFW_KEY_F)) {
		messageQueue->PS_SendMessage(Message(SYSTEM_SessionManager, SYSTEM_RenderEngine, MT_FullScreenToggle));
	}

	HandleMessages();
}

SessionResult MainMenuSession::Complete() {
	camera->SetStatic(false);
	return result;
}

void MainMenuSession::OnPlayButtonClick() {
	result = SR_START_SANDBOX;
	ShutDownSession();
}

void MainMenuSession::OnPhysBenchButtonClick() {
	result = SR_START_PHYSICS_BENCHMARK;
	ShutDownSession();
}

void MainMenuSession::OnGraphBenchButtonClick() {
	result = SR_START_GRAPHICS_BENCHMARK;
	ShutDownSession();
}

void MainMenuSession::OnExitButtonClick() {
	result = SR_EXIT_GAME;
	ShutDownSession();
}

void MainMenuSession::HandleEntityDestroyed(BaseEntity * entity) {
	if (entity->type == ET_World && world != nullptr && entity->id == world->id) {
		world->UnregisterAsObserver();
		world = nullptr;
	}

	if (entity->type == ET_ImageButton) {
		for (int i = 0; i < imageButtonEntities.size(); ++i) {
			if (imageButtonEntities[i]->id == entity->id) {
				imageButtonEntities[i]->UnregisterAsObserver();
				imageButtonEntities.erase(imageButtonEntities.begin() + i);
				return;
			}
		}		
	}
}

void MainMenuSession::HandleMessages() {
	Message message = messageQueue->PS_GetMessage(SYSTEM_SessionManager);
	while (!message.IsEmpty()) {
		switch (message.messageType) {
		case MT_Entity_Destroyed:
			HandleEntityDestroyed(static_cast<BaseEntity*>(message.payload));
		default:
			break;
		}
		message = messageQueue->PS_GetMessage(SYSTEM_SessionManager);
	}
}
