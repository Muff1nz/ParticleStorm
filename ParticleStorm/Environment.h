#pragma once
#include <SDL2/SDL.h>
#include <vec2.hpp>
#include <queue>
#include "QuadTree.h"
#include <mutex>

class QuadTree;

class Environment {
public:
	const int worldWidth = 2300;
	const int worldHeight = 1300;

	const int circleCount = 4000;
	const int circleRadius = 10;

	SDL_Color circleColor{};

	glm::vec2* circlePos;
	glm::vec2* circleVel;
	std::queue<glm::vec2> explosions;

	QuadTree* tree;
	std::mutex treeMutex{};

	int* renderOrder;
	std::mutex renderLock{};

	Environment();
	~Environment();

	void SwapParticles(int one, int two) const;
};

