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
	workerThreads.Init(workerThreadCount);
}


//Maybe optimize with XOR swapping?
void Environment::SwapParticles(const int one, const int two) {
	const auto tempPos = particlePos[one];
	particlePos[one] = particlePos[two];
	particlePos[two] = tempPos;

	const auto tempVel = particleVel[one];
	particleVel[one] = particleVel[two];
	particleVel[two] = tempVel;
}

