#include "SandboxSession.h"

#include <iostream>


#include "ImageButtonEntity.h"
#include "ParticlesEntity.h"

SandboxSession::SandboxSession(MessageSystem* messageQueue, EventEngine* eventEngine, Camera* camera, Stats* stats) : SessionManager(messageQueue, eventEngine, camera, stats) { }

SandboxSession::~SandboxSession() = default;

void SandboxSession::Init() {
	stats->ClearData();
	timer.Restart();

	camera->SetStatic(false);

	config.workerThreadCount = 14;
	config.screenHeight = 1200;
	config.screenWidth = 2800;
	messageQueue->PS_BroadcastMessage(Message(SYSTEM_SessionManager, MT_Config, static_cast<void*>(&config)));

	world = new WorldEntity(9000, 3780);
	
	particles = new ParticlesEntity(1000, 30);
	particles->Initialize(config.seed, world->width, world->height);

	world->RegisterAsObserver();
	particles->RegisterAsObserver();
	
	DeployEntity(world);
	DeployEntity(particles);
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
		if (debugQuadTree == nullptr) {
			debugQuadTree = new DebugQuadTreeEntity();
			debugQuadTree->RegisterAsObserver();
			DeployEntity(debugQuadTree);
		} else {
			RemoveEntity(debugQuadTree);
			debugQuadTree->UnregisterAsObserver();
			debugQuadTree = nullptr;
		}		
	}

	if (eventEngine->GetKeyDown(GLFW_KEY_F)) {
		messageQueue->PS_SendMessage(Message(SYSTEM_SessionManager, SYSTEM_RenderEngine, MT_FullScreenToggle));
	}

	HandleMessages();
}

SessionResult SandboxSession::Complete() {
	stats->CompleteSession();
	std::cout << stats->CompleteSessionToStringConsole();

	return SR_NONE;
}

void SandboxSession::HandleEntityDestroyed(BaseEntity* entity) {
	if (entity->type == ET_Particles && particles != nullptr && entity->id == particles->id) {
		particles->UnregisterAsObserver();
		particles = nullptr;
	}

	if (entity->type == ET_World && world != nullptr && entity->id == world->id) {
		world->UnregisterAsObserver();
		world = nullptr;
	}

	if (entity->type == ET_QuadTreeDebugEntity && debugQuadTree != nullptr && entity->id == debugQuadTree->id) {
		if (debugQuadTree->id == entity->id) {
			
			debugQuadTree->UnregisterAsObserver();
			debugQuadTree = nullptr;
		}
	}		
}

void SandboxSession::HandleMessages() {
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
