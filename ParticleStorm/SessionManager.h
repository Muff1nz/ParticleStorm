#pragma once

#include <string>
#include <vector>
#include "Camera.h"
#include "Stats.h"
#include "Timer.h"
#include "EntityEngine.h"
#include "ParticlesEntity.h"

class SessionManager {
public:
	SessionManager(MessageSystem* messageQueue, EventEngine* eventEngine, EntityEngine* entityEngine, Camera* camera, Stats* stats);
	~SessionManager();

	void InitSandbox();
	void SandboxUpdate();
	void CompleteSandbox() const;

private:
	const std::string shorTitle = "Graphics Bench";
	const std::string longTitle = "Graphics Bench";
	const std::string statsOutputDir = "C:/C++ Projects/ParticleStorm_Stats/";
	const std::string graphicsBenchGrapherDir = "C:/Python Projects/ParticleStorm_Tools/GraphicsBenchGrapher.py";
	const std::string multiStatsGrapherDir = "C:/Python Projects/ParticleStorm_Tools/MultiStatsGrapher.py";
	const std::string physicsDetailedGrapherDir = "C:/Python Projects/ParticleStorm_Tools/PhysicsDetailedGrapher.py";

	std::thread sessionThread;

	Timer timer;

	MessageSystem* messageQueue;
	EventEngine* eventEngine;
	EntityEngine* entityEngine;
	Camera* camera;	
	Stats* stats;

	WorldEntity* world;
	ParticlesEntity* particles;

	void OutputPhysBenchRunToFile(const std::string& sessionString) const;
	void OutputGraphBenchRunToFile(const std::string& sessionString) const;

	std::string SessionToString(const std::vector<std::string>& perSecondStats) const;
	static char* FileTime();

	std::string Benchmark(int particleCount, int particleRadius, int threadCount) const;

	void HandleMessages();

	void PhysicsBenchmark();
	void GraphicsBenchmark();
};

