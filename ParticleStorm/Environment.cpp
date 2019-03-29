#include "Environment.h"
#include <iostream>


Environment::Environment() {
	Init();
}

Environment::Environment(int circleCount, int circleRadius, int seed, int workerThreadCount) : particleCount(circleCount), particleRadius(circleRadius), workerThreadCount(workerThreadCount) {
	Init();
	this->seed = seed;
}

Environment::~Environment() {
	delete particlePos;
	delete particleVel;
}

void Environment::Init() {
	particlePos = new glm::vec2[particleCount];
	particleVel = new glm::vec2[particleCount];
	particleResting = new bool[particleCount];
	workerThreads.Init(workerThreadCount);
}
