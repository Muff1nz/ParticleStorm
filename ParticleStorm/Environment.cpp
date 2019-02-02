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
	//LockParticles(one, two);

	const auto tempPos = particlePos[one];
	particlePos[one] = particlePos[two];
	particlePos[two] = tempPos;

	const auto tempVel = particleVel[one];
	particleVel[one] = particleVel[two];
	particleVel[two] = tempVel;

	//UnlockParticles(one, two);
}

void Environment::LockParticles(int one, int two) const {
	//std::cout << "\n\n\n" << "ONE: " << one << " TWO: " << two << "\n\n\n";

	if (one < two) {
		particleLock[one].lock();
		particleLock[two].lock();
		return;
	}
	particleLock[two].lock();
	particleLock[one].lock();
}

void Environment::UnlockParticles(int one, int two) const {
	//std::cout << "\n\n\n" << "ONE: " << one << " TWO: " << two << "\n\n\n";

	if (one < two) {
		particleLock[two].unlock();
		particleLock[one].unlock();
		return;
	}
	particleLock[one].unlock();
	particleLock[two].unlock();
}

