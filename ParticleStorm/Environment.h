#pragma once

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
	Camera camera; //Move to core engine
		
	//World domain || Move to physics engine
	const int worldWidth = 9000; //21:9 ish, 2.38
	const int worldHeight = 3780;//	
	int seed{};

	//Runtime data domain 
	Stats stats{}; //Move to core engine

	//Particle domain || Move to physics engine
	const int particleCount = 10000;
	const float particleRadius = 20;

	glm::vec2* particlePos{};
	glm::vec2* particleVel{};
	bool* particleResting{};

	//Why is this here?
	std::atomic_int* particleQuadCount{};

	//Threading domain //Move to core engine
	bool done{}; //Get rid of this
	const int workerThreadCount = 14;
	WorkerThreadPool workerThreads;

	//Debugging domain //Dunno how to handle yet
	int debugQuadSize = 1000;
	glm::vec2* quadPos;
	glm::vec2* quadScale;

	Environment();
	Environment(int circleCount, int circleRadius, int seed, int workerThreadCount, int ww, int wh);
	~Environment(); 
private:
	void Init();
};

