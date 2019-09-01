#include "Environment.h"


Environment::Environment() {
	Init();
}

Environment::Environment(int circleCount, int circleRadius, int seed, int workerThreadCount, int sw, int sh, int ww, int wh) 
: particleCount(circleCount), particleRadius(circleRadius), workerThreadCount(workerThreadCount), screenWidth(sw), screenHeight(sh), worldWidth(ww), worldHeight(wh) {
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
	camera = Camera(worldHeight, worldWidth, screenHeight, screenWidth);
}
