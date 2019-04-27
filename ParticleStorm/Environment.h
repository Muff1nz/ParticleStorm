#pragma once

#include <queue>
#include <atomic>

#include "WorkerThreadPool.h"
#include "Stats.h"
#include "Camera.h"
#include <vec2.hpp>

class Environment {
public:

	const bool fullScreen = false;
	const int screenWidth = 2300;
	const int screenHeight = 1200;

	const int particleCount = 50000;
	const float particleRadius = 10;

	const int worldWidth = 9000;
	const int worldHeight = 6000;

	std::mutex renderLock;

	Camera camera;

	bool done{};
	int seed{};

	Stats stats{};

	glm::vec2* particlePos{};
	glm::vec2* particleVel{};
	bool* particleResting{};

	std::queue<glm::vec2> explosions;


	std::atomic_int* particleQuadCount{};

	const int workerThreadCount = 14;
	WorkerThreadPool workerThreads;

	Environment();
	Environment(int circleCount, int circleRadius, int seed, int workerThreadCount);
	~Environment(); 
private:
	void Init();
};

