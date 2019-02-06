#pragma once
#include <vec2.hpp>
#include <queue>
#include "QuadTree.h"
#include <mutex>
#include "WorkerThreadPool.h"

//#define full

class QuadTree;

class Environment {
public:

#ifdef full
	const bool fullScreen = true;
	const int worldWidth = 2560;
	const int worldHeight = 1440;
#else
	const bool fullScreen = false;
	const int worldWidth = 2300;
	const int worldHeight = 1300;
#endif

	const int particleCount = 15000;
	const int particleRadius = 5;

	bool done;
	int seed;

	glm::vec2* particlePos;
	glm::vec2* particleVel;
	std::queue<glm::vec2> explosions;

	QuadTree* tree;
	std::mutex treeMutex{};
	std::mutex renderLock{};

	const int workerThreadCount = 30;
	WorkerThreadPool workerThreads;

	Environment();
	Environment(int circleCount, int circleRadius, int seed, int workerThreadCount);
	~Environment(); 

	void SwapParticles(int one, int two);
private:
	void Init();
};

