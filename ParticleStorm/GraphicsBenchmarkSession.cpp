#include "GraphicsBenchmarkSession.h"

#include <iostream>


GraphicsBenchmarkSession::GraphicsBenchmarkSession(MessageSystem* messageQueue, EventEngine* eventEngine, Camera* camera, Stats* stats) : SessionManager(messageQueue, eventEngine, camera, stats) { }

GraphicsBenchmarkSession::~GraphicsBenchmarkSession() = default;;

void GraphicsBenchmarkSession::Init() {
	stats->ClearData();

	Timer::unhinged = true;
	currentPhase = -1;
	perSecondStats.clear();
	benchmarkString = "";

	config = new Configuration();
	config->workerThreadCount = threadCount;
	config->screenHeight = 1200;
	config->screenWidth = 2800;
	messageQueue->PS_BroadcastMessage(Message(SYSTEM_SessionManager, MT_Config, static_cast<void*>(config)));

	world = new WorldEntity(10000, 5450);
	world->RegisterAsObserver();
	DeployEntity(world);

	explosionIndex = 0;
	for (int i = 0; i < explosionPointCount; ++i) {
		explosionPoints[i] = { world->width * (float(i) / (explosionPointCount - 1)) , world->height};
	}	
}

void GraphicsBenchmarkSession::Update() {
	HandleMessages();
	
	if (currentPhase == phases) //Phase handling
		return;

	if (timer.ElapsedSeconds() >= 1) {
		timer.Restart();
		stats->CompleteLastSecond();
		std::cout << stats->LastSecondToStringConsole();
		perSecondStats.push_back(stats->LastSecondToString());

		std::cout << "\nBenchmark is " + std::to_string(perSecondStats.size() / float(phaseDuration) * 100) + "% complete\n";
	}
	
	//Phase handling
	if (currentPhase == -1 || perSecondStats.size() == phaseDuration) { //Phase handling
		if (particles != nullptr) {
			particles->UnregisterAsObserver();
			RemoveEntity(particles);
			particles = nullptr;
		}		
		
		if (perSecondStats.size() == phaseDuration) {
			benchmarkString += "<\n";
			benchmarkString += SessionToString(perSecondStats, longTitle, particleCounts[currentPhase], particleRadiuses[currentPhase]);
			benchmarkString += ">\n";
			perSecondStats.clear();
			stats->ClearData();
		}

		currentPhase++;
		timer.Restart();
		
		if (currentPhase == phases) {
			OutputSessionToFile(benchmarkString, shorTitle, statsOutputDir, { graphicsBenchGrapherDir });
			messageQueue->PS_BroadcastMessage(Message(SYSTEM_SessionManager, MT_Shutdown_Session));
			return;
		}		

		particles = new ParticlesEntity(particleCounts[currentPhase], particleRadiuses[currentPhase]);
		particles->Initialize(1337, world->width, world->height);
		particles->RegisterAsObserver();
		DeployEntity(particles);

		explosionIndex = 0;
		explosionTimer.Restart();		
	}

	if (explosionTimer.ElapsedSeconds() >= 1.0f) {
		explosionTimer.Restart();
		const auto impact = explosionPoints[explosionIndex++ % explosionPointCount];
		messageQueue->PS_SendMessage(Message(SYSTEM_SessionManager, SYSTEM_PhysicsEngine, MT_Explosion, new glm::vec2(impact.x, world->height - impact.y)));
	}
}

void GraphicsBenchmarkSession::Complete() {
	Timer::unhinged = false;
}

void GraphicsBenchmarkSession::HandleEntityDestroyed(BaseEntity* entity) {
	if (entity->type == ET_Particles && particles != nullptr && entity->id == particles->id) {
		particles->UnregisterAsObserver();
		particles = nullptr;
	}

	if (entity->type == ET_World && world != nullptr && entity->id == world->id) {
		world->UnregisterAsObserver();
		world = nullptr;
	}
}

void GraphicsBenchmarkSession::HandleMessages() {
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

//
//
//
//std::string SessionManager::Benchmark(int count, int radius, int threadCount) const {
//	Timer::unhinged = true;
//
//
//
//	std::vector<std::string> perSecondStats;
//
//	int explosionIndex = 0;
//	const int explosionPointCount = 4;
//	glm::vec2 explosionPoints[explosionPointCount];
//	for (int i = 0; i < explosionPointCount; ++i) {
//		explosionPoints[i] = { particles.worldWidth * (float(i) / (explosionPointCount - 1)) , particles.worldHeight };
//	}
//
//	Timer sessionTimer;
//	sessionTimer.Start();
//
//	Timer timer;
//	timer.Start();
//
//	Timer explosionTimer;
//	explosionTimer.Start();
//
//	const int benchmarkDuration = 10;
//
//	while (perSecondStats.size() < benchmarkDuration) {
//		std::this_thread::sleep_for(std::chrono::microseconds(10));
//
//		if (timer.ElapsedSeconds() >= 1) {
//			timer.Restart();
//			particles.stats.CompleteLastSecond();
//			std::cout << particles.stats.LastSecondToStringConsole();
//			perSecondStats.push_back(particles.stats.LastSecondToString());
//
//			std::cout << "\nBenchmark is " + std::to_string(perSecondStats.size() / float(benchmarkDuration) * 100) + "% complete\n";
//		}
//
//		if (explosionTimer.ElapsedSeconds() >= 1.0f) {
//			explosionTimer.Restart();
//			const auto impact = explosionPoints[explosionIndex++ % explosionPointCount];
//			messageQueue.PS_SendMessage(Message(SYSTEM_SessionManager, SYSTEM_PhysicsEngine, MT_Explosion, new glm::vec2(impact.x, particles.worldHeight - impact.y)));
//		}
//
//		glfwPollEvents();
//	}
//	particles.done = true;
//
//	particles.stats.CompleteSession();
//	std::cout << particles.stats.CompleteSessionToStringConsole();
//
//	physicsEngine.Join();
//	renderEngine.Join();
//
//	renderEngine.Dispose();
//
//	Timer::unhinged = false;
//
//	return SessionToString(perSecondStats, particles);
//}
//
//void SessionManager::PhysicsBenchmark() const {
//	const int threadRuns = 4;
//	const int particleRuns = 3;
//	int threadCounts[] = { 2, 4, 8, 14 };
//	int particleCounts[] = { 20000, 40000, 80000 };
//	int particleRadiuses[] = { 16, 12, 10 };
//
//	std::string sessionString = "";
//
//	for (int i = 0; i < particleRuns; ++i) {
//		for (int j = 0; j < threadRuns; ++j) {
//			sessionString += "<\n";
//			sessionString += Benchmark(particleCounts[i], particleRadiuses[i], threadCounts[j]);
//			sessionString += ">\n";
//		}
//	}
//	OutputPhysBenchRunToFile(sessionString);
//}
////
////void SessionManager::GraphicsBenchmark() const {
////	const int particleRuns = 3;
////	int threadCount = 6;
////	int particleCounts[] = { 20000, 40000, 80000 };
////	int particleRadiuses[] = { 16, 12, 10 };
////
////	std::string sessionString = "";
////
////	for (int i = 0; i < particleRuns; ++i) {
////		sessionString += "<\n";
////		sessionString += Benchmark(particleCounts[i], particleRadiuses[i], threadCount);
////		sessionString += ">\n";		
////	}
////	OutputGraphBenchRunToFile(sessionString);
////}