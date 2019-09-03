#pragma once

#include <queue>
#include <atomic>

#include "WorkerThreadPool.h"
#include "Stats.h"
#include "Camera.h"
#include <vec2.hpp>
#include "Rect.h"

class Environment {
public:

	const bool fullScreen = false;
	const int screenWidth = 3000;
	const int screenHeight = 1200;

	const int particleCount = 10000;
	const float particleRadius = 20;

	const int worldWidth = 9000;
	const int worldHeight = 3600;

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

	bool renderQuadTree = false;
	std::vector<Rect> quadRects;

	Environment();
	Environment(int circleCount, int circleRadius, int seed, int workerThreadCount, int sw, int sh, int ww, int wh);
	~Environment(); 
private:
	void Init();
};

