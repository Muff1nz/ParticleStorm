#pragma once
#include <SDL2/SDL.h>
#include <vec2.hpp>
#include <queue>
#include "QuadTree.h"
#include <mutex>

class QuadTree;

class Environment {
public:
	const int worldWidth = 2400;
	const int worldHeight = 1300;

	const int circleCount = 100;
	const int circleRadius = 32;

	SDL_Color circleColor{};

	glm::vec2* circlePos;
	glm::vec2* circleVel;
	std::queue<glm::vec2> explosions;

	QuadTree* tree;
	std::mutex treeMutex{};

	Environment();
	~Environment();

	void SwapParticles(int one, int two) const;
};

