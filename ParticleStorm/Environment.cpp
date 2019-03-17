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
	delete shadowParticlePos;
	delete shadowParticleVel;
}

void Environment::Init() {
	particlePos = new glm::vec2[particleCount];
	particleVel = new glm::vec2[particleCount];
	particleResting = new bool[particleCount];
	shadowParticlePos = new glm::vec2[particleCount];
	shadowParticleVel = new glm::vec2[particleCount];
	workerThreads.Init(workerThreadCount);
}
