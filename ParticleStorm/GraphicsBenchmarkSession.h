#pragma once
#include "SessionManager.h"
#include "ParticlesEntity.h"

class GraphicsBenchmarkSession : public SessionManager {
public:
	GraphicsBenchmarkSession(MessageSystem* messageQueue, EventEngine* eventEngine, Camera* camera, Stats* stats);
	~GraphicsBenchmarkSession();
	
	void Init() override;
	void Update() override;
	SessionResult Complete() override;

private:
	const std::string shorTitle = "Graphics Bench";
	const std::string longTitle = "Graphics Bench";
	const std::string statsOutputDir = "C:/C++ Projects/ParticleStorm_Stats/";
	const std::string graphicsBenchGrapherDir = "C:/Python Projects/ParticleStorm_Tools/GraphicsBenchGrapher.py";

	WorldEntity* world = nullptr;
	ParticlesEntity* particles = nullptr;
	
	const int phaseDuration = 10; //Number of one second stats per phase
	int phases = 3;
	int currentPhase = 0;

	int particleCounts[3] = { 20000, 40000, 80000 };
	int particleRadiuses[3] = { 16, 12, 10 };

	int threadCount = 6;

	std::vector<std::string> perSecondStats;
	std::string benchmarkString;

	Timer explosionTimer;
	const int explosionPointCount = 4;
	glm::vec2 explosionPoints[4];
	int explosionIndex;


	void HandleEntityDestroyed(BaseEntity* entity);
	void HandleMessages() override;
};

