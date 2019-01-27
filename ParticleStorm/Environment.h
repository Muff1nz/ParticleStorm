#pragma once
#include <SDL2/SDL.h>
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

	const int circleCount = 10;
	const int circleRadius = 16;

	int seed;

	SDL_Color circleColor{};

	glm::vec2* circlePos;
	glm::vec2* circleVel;
	std::queue<glm::vec2> explosions;

	TestSquare square{};

	QuadTree* tree;
	std::mutex treeMutex{};

	std::mutex renderLock{};

	Environment();
	Environment(int circleCount, int circleRadius, int seed);
	~Environment();
 
	void Init();
	void SwapParticles(int one, int two) const;
};

