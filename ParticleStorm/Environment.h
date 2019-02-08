#pragma once
#include <vec2.hpp>
#include <queue>
#include "QuadTree.h"
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

	const int particleCount = 40000;
	const int particleRadius = 4;

	bool done;
	int seed;

	glm::vec2* particlePos;
	glm::vec2* particleVel;
	std::queue<glm::vec2> explosions;

	QuadTree* tree;

	const int workerThreadCount = 30;
	WorkerThreadPool workerThreads;

	Environment();
	Environment(int circleCount, int circleRadius, int seed, int workerThreadCount);
	~Environment(); 
private:
	void Init();
};

