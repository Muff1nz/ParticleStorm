#pragma once

#include <string>
#include <vector>
#include "Camera.h"
#include "Stats.h"
#include "Timer.h"
#include "EntityEngine.h"

class SessionManager {
public:
	SessionManager(MessageSystem* messageQueue, EventEngine* eventEngine, Camera* camera, Stats* stats);
	~SessionManager();

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Complete() = 0;

protected:
	

	Timer timer;

	Configuration* config;

	MessageSystem* messageQueue;
	EventEngine* eventEngine;
	Camera* camera;
	Stats* stats;

	virtual void HandleMessages() = 0;

	static char* FileTime();
	void OutputSessionToFile(const std::string& sessionString, std::string shortTitle, std::string statsOutputDir, std::vector<std::string> graphScripts);
	std::string SessionToString(const std::vector<std::string>& perSecondStats, std::string longTitle) const;
private:
	//const std::string shorTitle = "Graphics Bench";
	//const std::string longTitle = "Graphics Bench";
	//const std::string statsOutputDir = "C:/C++ Projects/ParticleStorm_Stats/";
	//const std::string graphicsBenchGrapherDir = "C:/Python Projects/ParticleStorm_Tools/GraphicsBenchGrapher.py";
	//const std::string multiStatsGrapherDir = "C:/Python Projects/ParticleStorm_Tools/MultiStatsGrapher.py";
	//const std::string physicsDetailedGrapherDir = "C:/Python Projects/ParticleStorm_Tools/PhysicsDetailedGrapher.py";



	//void OutputPhysBenchRunToFile(const std::string& sessionString) const;
	//void OutputGraphBenchRunToFile(const std::string& sessionString) const;



	

};

