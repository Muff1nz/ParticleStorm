#pragma once

#include <string>
#include <vector>
#include "Camera.h"
#include "Stats.h"
#include "Timer.h"
#include "EntityEngine.h"
#include "Configuration.h"
#include "SessionResult.h"

class SessionManager {
public:
	SessionManager(MessageSystem* messageQueue, EventEngine* eventEngine, Camera* camera, Stats* stats);
	virtual ~SessionManager();

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual SessionResult Complete() = 0;

protected:
	Timer timer;

	Configuration config;

	MessageSystem* messageQueue;
	EventEngine* eventEngine;
	Camera* camera;
	Stats* stats;

	virtual void HandleMessages() = 0;

	void ShutDownSession() const;

	void DeployEntity(BaseEntity* entity) const;
	void RemoveEntity(BaseEntity* entity) const;

	static char* FileTime();
	void OutputSessionToFile(const std::string& sessionString, std::string shortTitle, std::string statsOutputDir, std::vector<std::string> graphScripts);
	std::string SessionToString(const std::vector<std::string>& perSecondStats, std::string longTitle, int particleCount, int particleRadius) const;
};

