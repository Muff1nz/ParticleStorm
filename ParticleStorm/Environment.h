#pragma once

#include <queue>
#include <atomic>

#include "WorkerThreadPool.h"
#include "Stats.h"
#include "Camera.h"
#include <vec2.hpp>
#include "Rect.h"

//TODO: Split into more classes, all contained inside Environment
//TODO: Try to have as little as possible inside here
class Environment {
public:
	//Rendering domain
	Camera camera;
	const bool fullScreen = false;
	const int screenWidth = 3000;
	const int screenHeight = 1200;
		
	//World domain
	const int worldWidth = 9000;
	const int worldHeight = 3600;	
	int seed{};

	//Runtime data domain
	Stats stats{};

	//Particle domain
	const int particleCount = 10000;
	const float particleRadius = 20;

	glm::vec2* particlePos{};
	glm::vec2* particleVel{};
	bool* particleResting{};

	//Event domain
	std::queue<glm::vec2> explosions;

	//Why is this here?
	std::atomic_int* particleQuadCount{};

	//Threading domain
	bool done{};
	const int workerThreadCount = 14;
	WorkerThreadPool workerThreads;

	//Debugging domain
	bool runtimeDebugMode = false;
	std::vector<Rect> quadRects;

	Environment();
	Environment(int circleCount, int circleRadius, int seed, int workerThreadCount, int sw, int sh, int ww, int wh);
	~Environment(); 
private:
	void Init();
};

