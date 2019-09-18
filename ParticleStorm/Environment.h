#pragma once

#include <queue>
#include <atomic>

#include "WorkerThreadPool.h"
#include "Stats.h"
#include "Camera.h"
#include <vec2.hpp>

//TODO: Split into more classes, all contained inside Environment
//TODO: Try to have as little as possible inside here
class Environment {
public:
	//Rendering domain
	Camera camera;
	const bool fullScreen = false;
		
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

	//Why is this here?
	std::atomic_int* particleQuadCount{};

	//Threading domain
	bool done{};
	const int workerThreadCount = 14;
	WorkerThreadPool workerThreads;

	//Debugging domain
	int debugQuadSize = 1000;
	glm::vec2* quadPos;
	glm::vec2* quadScale;

	Environment();
	Environment(int circleCount, int circleRadius, int seed, int workerThreadCount, int ww, int wh);
	~Environment(); 
private:
	void Init();
};

