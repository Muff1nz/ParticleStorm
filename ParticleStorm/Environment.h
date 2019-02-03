#pragma once
#include <vec2.hpp>
#include <queue>
#include "QuadTree.h"
#include <mutex>
#include "WorkerThreadPool.h"

class QuadTree;

class Environment {
public:
	const int worldWidth = 2300;
	const int worldHeight = 1300;

	const int particleCount = 10000;
	const int particleRadius = 6;

	bool done;
	int seed;

	glm::vec2* particlePos;
	glm::vec2* particleVel;
	std::queue<glm::vec2> explosions;

	QuadTree* tree;
	std::mutex treeMutex{};
	std::mutex renderLock{};
	std::vector<std::unique_ptr<std::mutex>> particleLock;

	const int workerThreadCount = 30;
	WorkerThreadPool workerThreads;

	Environment();
	Environment(int circleCount, int circleRadius, int seed);
	~Environment(); 

	void SwapParticles(int one, int two);
	void LockParticles(int one, int two) ;
	void UnlockParticles(int one, int two);
	void LockParticle(int particle);
	void UnlockParticle(int particle);

private:
	void Init();
};

