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

SessionManager::~SessionManager() = default;

char* SessionManager::FileTime() {
	char name[20];
	time_t now = time(0);
	struct tm buf;
	localtime_s(&buf, &now);
	strftime(name, sizeof(name), "%d%m%Y_%H%M%S", &buf);
	return name;
}

void SessionManager::OutputSessionToFile(const std::string& sessionString, std::string shortTitle, std::string statsOutputDir, std::vector<std::string> graphScripts) {
	const std::string statsOutputFolder = statsOutputDir + shortTitle + "_" + FileTime();
	const std::string statsOutputFilePath = statsOutputFolder + "/" + shortTitle + "_" + FileTime() + ".txt";

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

std::string SessionManager::SessionToString(const std::vector<std::string>& perSecondStats, std::string longTitle, int particleCount, int particleRadius) const {
	std::string sessionString;
	sessionString += "Title: " + longTitle + "\n";
	sessionString += "Simulated " + std::to_string(particleCount) + " particles with a raidus of: " + std::to_string(particleRadius) + "\n";
	sessionString += "Worker threads: " + std::to_string(config.workerThreadCount) + "\n";
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


void SessionManager::ShutDownSession() const {
	messageQueue->PS_BroadcastMessage(Message(SYSTEM_SessionManager, MT_Shutdown_Session));
}

void SessionManager::DeployEntity(BaseEntity* entity) const {
	messageQueue->PS_SendMessage(Message(SYSTEM_SessionManager, SYSTEM_EntityEngine, MT_Entity_Submit_Request, entity));
}

void SessionManager::RemoveEntity(BaseEntity* entity) const {
	messageQueue->PS_SendMessage(Message(SYSTEM_SessionManager, SYSTEM_EntityEngine, MT_Entity_Destroy_Request, entity));
}