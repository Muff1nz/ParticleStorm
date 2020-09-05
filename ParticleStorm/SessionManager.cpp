#include "SessionManager.h"

#include <iostream>
#include <fstream>
#include <Windows.h>

#include "PhysicsEngine.h"
#include "RenderEngineVulkan.h"
#include "Utils.h"
#include "EntityEngine.h"


SessionManager::SessionManager(MessageSystem* messageQueue, EventEngine* eventEngine, Camera* camera, Stats* stats) {
	this->messageQueue = messageQueue;
	this->eventEngine = eventEngine;
	this->camera = camera;
	this->stats = stats;
}

SessionManager::~SessionManager() {
	delete config;
};

char* SessionManager::FileTime() {
	char name[20];
	time_t now = time(0);
	struct tm buf;
	localtime_s(&buf, &now);
	strftime(name, sizeof(name), "%d%m%Y_%H%M%S", &buf);
	return name;
}

void SessionManager::OutputSessionToFile(const std::string& sessionString, std::string shortTitle, std::string statsOutputDir, std::vector<std::string> graphScripts) {
	const std::string fileLeadText = "GraphBench_";
	const std::string statsOutputFolder = statsOutputDir + fileLeadText + shortTitle + "_" + FileTime();
	const std::string statsOutputFilePath = statsOutputFolder + "/" + fileLeadText + shortTitle + "_" + FileTime() + ".txt";

	std::cout << "Results are saved to: " + statsOutputFolder + "\n";
	CreateDirectory(statsOutputFolder.c_str(), nullptr);
	std::ofstream statsFile(statsOutputFilePath);

	statsFile << sessionString;

	statsFile.close();

	for (auto graphScript : graphScripts) {
		auto command = "python \"" + graphScript + "\" \"" + statsOutputFilePath + "\"";
		std::cout << command + "\n";
		system(command.c_str());
	}
}

std::string SessionManager::SessionToString(const std::vector<std::string>& perSecondStats, std::string longTitle) const {
	std::string sessionString;
	sessionString += "Title: " + longTitle + "\n";
	sessionString += "Simulated " + std::to_string(config->particleCount) + " particles with a raidus of: " + std::to_string(config->particleRadius) + "\n";
	sessionString += "Worker threads: " + std::to_string(config->workerThreadCount) + "\n";
	sessionString += "Duration: " + std::to_string(perSecondStats.size()) + " seconds\n";
	sessionString += "[\n";
	sessionString += stats->CompleteSessionToString();
	sessionString += "]\n";
	for (int i = 0; i < perSecondStats.size(); i++) {
		sessionString += "{\n";
		sessionString += std::to_string(i + 1) + "\n";
		sessionString += perSecondStats[i];
		sessionString += "}\n";
	}
	return sessionString;
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