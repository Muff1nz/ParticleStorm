#include "Environment.h"
#include <iostream>


Environment::Environment() {
	Init();
}

Environment::Environment(int circleCount, int circleRadius, int seed) : particleCount(circleCount), particleRadius(circleRadius) {
	Init();
	this->seed = seed;
}

Environment::~Environment() = default;

void Environment::Init() {
	particlePos = new glm::vec2[particleCount];
	particleVel = new glm::vec2[particleCount];
	particleLock = new std::mutex[particleCount];
}


//Maybe optimize with XOR swapping?
void Environment::SwapParticles(const int one, const int two) const {
	const auto tempPos = particlePos[one];
	particlePos[one] = particlePos[two];
	particlePos[two] = tempPos;

	const auto tempVel = particleVel[one];
	particleVel[one] = particleVel[two];
	particleVel[two] = tempVel;
}

