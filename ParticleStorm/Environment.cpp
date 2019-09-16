#include "Environment.h"


Environment::Environment() {
	Init();
}

Environment::Environment(int circleCount, int circleRadius, int seed, int workerThreadCount, int ww, int wh) 
: worldWidth(ww), worldHeight(wh), particleCount(circleCount), particleRadius(circleRadius), workerThreadCount(workerThreadCount) {
	Init();
	this->seed = seed;
}

Environment::~Environment() {
	delete particlePos;
	delete particleVel;
	delete particleQuadCount;
}

void Environment::Init() {
	particlePos = new glm::vec2[particleCount];
	particleVel = new glm::vec2[particleCount];
	particleResting = new bool[particleCount];
	particleQuadCount = new std::atomic_int[particleCount];
	workerThreads.Init(workerThreadCount);

	quadPos = new glm::vec2[debugQuadSize];
	quadScale = new glm::vec2[debugQuadSize];
}
