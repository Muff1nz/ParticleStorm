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

	const int circleCount = 1000;
	const int circleRadius = 16;

	SDL_Color circleColor{};

	glm::vec2* circlePos;
	glm::vec2* circleVel;
	std::queue<glm::vec2> explosions;

	QuadTree* tree;
	std::mutex treeMutex{};

	int* renderOrder;
	std::mutex renderLock{};

	Environment();
	Environment(int circleCount, int circleRadius);
	~Environment();

	void Init();
	void SwapParticles(int one, int two) const;
};

