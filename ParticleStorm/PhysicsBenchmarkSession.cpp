#include "PhysicsBenchmarkSession.h"

#include <iostream>


PhysicsBenchmarkSession::PhysicsBenchmarkSession(MessageSystem* messageQueue, EventEngine* eventEngine, Camera* camera, Stats* stats) : SessionManager(messageQueue, eventEngine, camera, stats) { }

PhysicsBenchmarkSession::~PhysicsBenchmarkSession() = default;

void PhysicsBenchmarkSession::Init() {
	stats->ClearData();

	camera->SetStatic(true);

	Timer::unhinged = true;
	currentParticlePhase = 0;
	currentThreadPhase = -1;
	perSecondStats.clear();
	benchmarkString = "";

	config.screenHeight = 1200;
	config.screenWidth = 2800;

	world = new WorldEntity(10000, 5450);
	world->RegisterAsObserver();
	DeployEntity(world);

	explosionIndex = 0;
	for (int i = 0; i < explosionPointCount; ++i) {
		explosionPoints[i] = { world->width * (float(i) / (explosionPointCount - 1)) , world->height };
	}
}

void PhysicsBenchmarkSession::Update() {
	HandleMessages();

	if (currentParticlePhase == particlePhases) //Phase handling
		return;

	if (timer.ElapsedSeconds() >= 1) {
		timer.Restart();
		stats->CompleteLastSecond();
		std::cout << stats->LastSecondToStringConsole();
		perSecondStats.push_back(stats->LastSecondToString());

		std::cout << "\nBenchmark is " + std::to_string(perSecondStats.size() / float(phaseDuration) * 100) + "% complete\n";
	}

	//Phase handling
	if (currentThreadPhase == -1 || perSecondStats.size() == phaseDuration) { //Phase handling
		currentThreadPhase++;	
		
				
		if (particles != nullptr) {
			particles->UnregisterAsObserver();
			RemoveEntity(particles);
			particles = nullptr;
		}

		if (perSecondStats.size() == phaseDuration) {
			benchmarkString += "<\n";
			benchmarkString += SessionToString(perSecondStats, longTitle, particleCounts[currentParticlePhase], particleRadiuses[currentParticlePhase]);
			benchmarkString += ">\n";
			perSecondStats.clear();
			stats->ClearData();

			if (currentThreadPhase == threadPhases) {
				currentThreadPhase = 0;
				currentParticlePhase++;
			}
		}

		timer.Restart();

		if (currentParticlePhase == particlePhases) {
			OutputSessionToFile(benchmarkString, shorTitle, statsOutputDir, { multiStatsGrapherDir, physicsDetailedGrapherDir });
			messageQueue->PS_BroadcastMessage(Message(SYSTEM_SessionManager, MT_Shutdown_Session));
			return;
		}

		config.workerThreadCount = threadCounts[currentThreadPhase];		
		messageQueue->PS_BroadcastMessage(Message(SYSTEM_SessionManager, MT_Config, static_cast<void*>(&config)));
		
		particles = new ParticlesEntity(particleCounts[currentParticlePhase], particleRadiuses[currentParticlePhase]);
		particles->Initialize(1337, world->width, world->height);
		particles->RegisterAsObserver();
		DeployEntity(particles);

		explosionIndex = 0;
		explosionTimer.Restart();
	}

	if (explosionTimer.ElapsedSeconds() >= 1.0) {
		explosionTimer.Restart();
		const auto impact = explosionPoints[explosionIndex++ % explosionPointCount];
		messageQueue->PS_SendMessage(Message(SYSTEM_SessionManager, SYSTEM_PhysicsEngine, MT_Explosion, new glm::vec2(impact.x, world->height - impact.y)));
	}
}

SessionResult PhysicsBenchmarkSession::Complete() {
	camera->SetStatic(false);
	Timer::unhinged = false;
	return SR_NONE;
}

void PhysicsBenchmarkSession::HandleEntityDestroyed(BaseEntity * entity) {
	if (entity->type == ET_Particles && particles != nullptr && entity->id == particles->id) {
		particles->UnregisterAsObserver();
		particles = nullptr;
	}

	if (entity->type == ET_World && world != nullptr && entity->id == world->id) {
		world->UnregisterAsObserver();
		world = nullptr;
	}
}

void PhysicsBenchmarkSession::HandleMessages() {
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