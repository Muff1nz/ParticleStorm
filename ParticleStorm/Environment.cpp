#include "Environment.h"
#include <iostream>


Environment::Environment() {
	Init();
}

Environment::Environment(int circleCount, int circleRadius, int seed) : particleCount(circleCount), particleRadius(circleRadius) {
	Init();
	this->seed = seed;
}

Environment::~Environment() {
	delete particlePos;
	delete particleVel;
	//for (int i = 0; i < particleCount; ++i) {
	//	delete particleLock[i];
	//}
	//delete particleLock;
}

void Environment::Init() {
	particlePos = new glm::vec2[particleCount];
	particleVel = new glm::vec2[particleCount];
	particleLock.resize(particleCount);
	for (int i = 0; i < particleCount; ++i) {
		particleLock.push_back(std::make_unique<std::mutex>());
	}
	workerThreads.Init(workerThreadCount, &done);
}


//Maybe optimize with XOR swapping?
void Environment::SwapParticles(const int one, const int two) {
	//LockParticles(one, two);
	//std::cout << "\n\n\n" << "ONE: " << one << " TWO: " << two << "\n\n\n";
	const auto tempPos = particlePos[one];
	particlePos[one] = particlePos[two];
	particlePos[two] = tempPos;

	const auto tempVel = particleVel[one];
	particleVel[one] = particleVel[two];
	particleVel[two] = tempVel;

	//UnlockParticles(one, two);
}

void Environment::LockParticles(int one, int two) {
	//std::cout << "\n\n\n" << "ONE: " << one << " TWO: " << two << "\n\n\n";

	if (one == two) {
		particleLock[one]->lock();
		return;
	}

	if (one < two) {
		particleLock[one]->lock();
		particleLock[two]->lock();
		return;
	}
	particleLock[two]->lock();
	particleLock[one]->lock();
}

void Environment::UnlockParticles(int one, int two) {
	//std::cout << "\n\n\n" << "ONE: " << one << " TWO: " << two << "\n\n\n";
	
	if (one == two) {
		particleLock[one]->unlock();
		return;
	}

	if (one < two) {
		particleLock[two]->unlock();
		particleLock[one]->unlock();
		return;
	}
	particleLock[one]->unlock();
	particleLock[two]->unlock();
}

void Environment::LockParticle(int particle) {
	particleLock[particle]->lock();
}

void Environment::UnlockParticle(int particle) {
	particleLock[particle]->unlock();
}

