#include "SessionManager.h"

#include <iostream>
#include <fstream>
#include <windows.h>

#include "Timer.h"
#include "PhysicsEngine.h"
#include "RenderEngineVulkan.h"
#include "Utils.h"
#include "EntityEngine.h"


SessionManager::SessionManager(MessageSystem* messageQueue, EventEngine* eventEngine, EntityEngine* entityEngine, Camera* camera, Stats* stats) {
	this->messageQueue = messageQueue;
	this->eventEngine = eventEngine;
	this->entityEngine = entityEngine;
	this->camera = camera;
	this->stats = stats;
}

SessionManager::~SessionManager() = default;

char* SessionManager::FileTime() {
	char name[20];
	time_t now = time(0);
	struct tm buf;
	localtime_s(&buf, &now);
	strftime(name, sizeof(name), "%d%m%Y_%H%M%S", &buf);
	return name;
}

void SessionManager::OutputPhysBenchRunToFile(const std::string& sessionString) const {
	const std::string fileLeadText = "PhysBench_";
	const std::string statsOutputFolder = statsOutputDir + fileLeadText + shorTitle + "_" + FileTime();
	const std::string statsOutputFilePath = statsOutputFolder + "/" + fileLeadText + shorTitle + "_" + FileTime() + ".txt";

	std::cout << "Results are saved to: " + statsOutputFolder + "\n";
	CreateDirectory(statsOutputFolder.c_str(), nullptr);
	std::ofstream statsFile(statsOutputFilePath);

	statsFile << sessionString;

	statsFile.close();

	auto command = "python \"" + multiStatsGrapherDir + "\" \"" + statsOutputFilePath + "\"";
	std::cout << command + "\n";
	system(command.c_str());

	command = "python \"" + physicsDetailedGrapherDir + "\" \"" + statsOutputFilePath + "\"";
	std::cout << command + "\n";
	system(command.c_str());
}

void SessionManager::OutputGraphBenchRunToFile(const std::string& sessionString) const {
	const std::string fileLeadText = "GraphBench_";
	const std::string statsOutputFolder = statsOutputDir + fileLeadText + shorTitle + "_" + FileTime();
	const std::string statsOutputFilePath = statsOutputFolder + "/" + fileLeadText + shorTitle + "_" + FileTime() + ".txt";

	std::cout << "Results are saved to: " + statsOutputFolder + "\n";
	CreateDirectory(statsOutputFolder.c_str(), nullptr);
	std::ofstream statsFile(statsOutputFilePath);

	statsFile << sessionString;

	statsFile.close();

	auto command = "python \"" + graphicsBenchGrapherDir + "\" \"" + statsOutputFilePath + "\"";
	std::cout << command + "\n";
	system(command.c_str());
}

//std::string SessionManager::SessionToString(const std::vector<std::string>& perSecondStats) const {
//	std::string sessionString;
//	sessionString += "Title: " + longTitle + "\n";
//	sessionString += "Simulated " + std::to_string(particles.count) + " particles with a raidus of: " + std::to_string(particles.radius) + "\n";
//	sessionString += "Worker threads: " + std::to_string(particles.workerThreadCount) + "\n";
//	sessionString += "Duration: " + std::to_string(perSecondStats.size()) + " seconds\n";
//	sessionString += "[\n";
//	sessionString += stats->CompleteSessionToString();
//	sessionString += "]\n";
//	for (int i = 0; i < perSecondStats.size(); i++) {
//		sessionString += "{\n";
//		sessionString += std::to_string(i + 1) + "\n";
//		sessionString += perSecondStats[i];
//		sessionString += "}\n";
//	}
//	return sessionString;
//}

void SessionManager::HandleMessages() {
	Message message = messageQueue->PS_GetMessage(SYSTEM_SessionManager);
	while (!message.IsEmpty()) {
		switch (message.messageType) {
		default:
			break;
		}
		message = messageQueue->PS_GetMessage(SYSTEM_SessionManager);
	}	
}

void SessionManager::InitSandbox() {
	timer.Restart();

	world = new WorldEntity();
	world->id = entityEngine->GenerateUniqueId();
	world->width = 9000;
	world->height = 3780;
	world->position = new glm::vec2(world->width / 2, world->height / 2);
	world->scale = new glm::vec2(world->width / 2, world->height / 2);
	world->texturePath = "Textures/BackGround.png";

	entityEngine->SubmitEntity(world);

	particles = new ParticlesEntity();
	particles->id = entityEngine->GenerateUniqueId();
	particles->count = 20000;
	particles->radius = 10;
	particles->position = new glm::vec2[particles->count];
	particles->velocity = new glm::vec2[particles->count];
	particles->scale = new glm::vec2(particles->radius, particles->radius);
	particles->texturePath = "Textures/Circle.png";

	auto rng = NumberGenerator();

	const int maxSpeed = 1000;
	const auto circlePos = particles->position;
	const auto circleVel = particles->velocity;

	for (int i = 0; i < particles->count; i++) { //Initialize particles (position/velocity)
		circlePos[i] = glm::vec2(rng.GenerateFloat(0, world->width), rng.GenerateFloat(0, world->height));

		do {
			circleVel[i] = glm::vec2(rng.GenerateFloat(-maxSpeed, maxSpeed), rng.GenerateFloat(-maxSpeed, maxSpeed));
		} while (abs(circleVel[i].x) < 1 && abs(circleVel[i].y) < 1);
	}

	entityEngine->SubmitEntity(particles);
}

void SessionManager::SandboxUpdate() {
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

void SessionManager::CompleteSandbox() const {
	entityEngine->DestroyEntity(world);
	entityEngine->DestroyEntity(particles);

	stats->CompleteSession();
	std::cout << stats->CompleteSessionToStringConsole();
}


//std::string SessionManager::Benchmark(int count, int radius, int threadCount) const {
//	Timer::unhinged = true;
//
//	Environment particles(count, radius, 1337, threadCount, 10000, 5450);
//	MessageSystem messageQueue{};
//	RenderEngineVulkan renderEngine(&particles, &messageQueue);
//	PhysicsEngine physicsEngine(&particles, &messageQueue);
//	renderEngine.Init();
//	physicsEngine.Init();
//
//	renderEngine.Start();
//	physicsEngine.Start();
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
//
//void SessionManager::GraphicsBenchmark() const {
//	const int particleRuns = 3;
//	int threadCount = 6;
//	int particleCounts[] = { 20000, 40000, 80000 };
//	int particleRadiuses[] = { 16, 12, 10 };
//
//	std::string sessionString = "";
//
//	for (int i = 0; i < particleRuns; ++i) {
//		sessionString += "<\n";
//		sessionString += Benchmark(particleCounts[i], particleRadiuses[i], threadCount);
//		sessionString += ">\n";		
//	}
//	OutputGraphBenchRunToFile(sessionString);
//}