#pragma once
#include "SessionManager.h"
#include "ParticlesEntity.h"

class PhysicsBenchmarkSession : public SessionManager {
public:
	PhysicsBenchmarkSession(MessageSystem* messageQueue, EventEngine* eventEngine, Camera* camera, Stats* stats);
	~PhysicsBenchmarkSession();
	
	void Init() override;
	void Update() override;
	SessionResult Complete() override;
private:
	const std::string shorTitle = "Physics Bench";
	const std::string longTitle = "Physics Bench";
	const std::string statsOutputDir = "C:/C++ Projects/ParticleStorm_Stats/";
	const std::string multiStatsGrapherDir = "C:/Python Projects/ParticleStorm_Tools/MultiStatsGrapher.py";
	const std::string physicsDetailedGrapherDir = "C:/Python Projects/ParticleStorm_Tools/PhysicsDetailedGrapher.py";

	WorldEntity* world = nullptr;
	ParticlesEntity* particles = nullptr;

	const int phaseDuration = 10; //Number of one second stats per phase

	int threadPhases = 4;
	int currentThreadPhase = 0;
	int threadCounts[4] = { 2, 4, 8, 14 };
	
	int particlePhases = 3;
	int currentParticlePhase = 0;

	int particleCounts[3] = { 20000, 40000, 80000 };
	int particleRadiuses[3] = { 16, 12, 10 };	

	std::vector<std::string> perSecondStats;
	std::string benchmarkString;

	Timer explosionTimer;
	const int explosionPointCount = 4;
	glm::vec2 explosionPoints[4];
	int explosionIndex;


	void HandleEntityDestroyed(BaseEntity* entity);
	void HandleMessages() override;
};

