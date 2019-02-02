#pragma once
#include <vec2.hpp>
#include <queue>
#include "QuadTree.h"
#include <mutex>
#include "TestSquare.h"

class QuadTree;

class Environment {
public:
	const int worldWidth = 2300;
	const int worldHeight = 1300;

	const int particleCount = 8000;
	const int particleRadius = 8;

	int seed;

	glm::vec2* particlePos;
	glm::vec2* particleVel;
	std::queue<glm::vec2> explosions;

	TestSquare square{};

	QuadTree* tree;
	std::mutex treeMutex{};
	std::mutex renderLock{};
	std::mutex* particleLock;

	Environment();
	Environment(int circleCount, int circleRadius, int seed);
	~Environment();
 
	void Init();
	void SwapParticles(int one, int two) const;
};

