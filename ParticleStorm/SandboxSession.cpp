#include "SandboxSession.h"

#include <iostream>

SandboxSession::SandboxSession(MessageSystem* messageQueue, EventEngine* eventEngine, Camera* camera, Stats* stats) : SessionManager(messageQueue, eventEngine, camera, stats) { }

SandboxSession::~SandboxSession() = default;;

void SandboxSession::Init() {
	timer.Restart();

	config = new Configuration();

	config->workerThreadCount = 14;

	config->screenHeight = 1200;
	config->screenWidth = 2800;

	config->worldWidth = 9000;
	config->worldHeight = 3780;

	config->particleCount = 20000;
	config->particleRadius = 10;

	messageQueue->PS_BroadcastMessage(Message(SYSTEM_SessionManager, MT_Config, static_cast<void*>(config)));
}

void SandboxSession::Update() {
	if (timer.ElapsedSeconds() >= 1) {
		timer.Restart();
		stats->CompleteLastSecond();
		std::cout << stats->LastSecondToStringConsole();
	}

	if (eventEngine->GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
		glm::vec2 mouseWorldPos = camera->GetWorldPos(eventEngine->GetMousePos());
		messageQueue->PS_SendMessage(Message(SYSTEM_SessionManager, SYSTEM_PhysicsEngine, MT_Explosion, new glm::vec2(mouseWorldPos)));
		++stats->explosionTotalLastSecond;
	}

	if (eventEngine->GetKeyDown(GLFW_KEY_Q)) {
		messageQueue->PS_BroadcastMessage(Message(SYSTEM_SessionManager, MT_DebugModeToggle));
	}

	if (eventEngine->GetKeyDown(GLFW_KEY_F)) {
		messageQueue->PS_SendMessage(Message(SYSTEM_SessionManager, SYSTEM_RenderEngine, MT_FullScreenToggle));
	}

	HandleMessages();
}

void SandboxSession::Complete() {
	stats->CompleteSession();
	std::cout << stats->CompleteSessionToStringConsole();
}

void SandboxSession::HandleMessages() {
	Message message = messageQueue->PS_GetMessage(SYSTEM_SessionManager);
	while (!message.IsEmpty()) {
		switch (message.messageType) {
		default:
			break;
		}
		message = messageQueue->PS_GetMessage(SYSTEM_SessionManager);
	}
}
