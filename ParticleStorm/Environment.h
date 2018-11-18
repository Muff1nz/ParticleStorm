#pragma once
#include <SDL2/SDL.h>
#include <vec2.hpp>
#include <queue>

class Environment {
public:
	const int worldWidth = 2400;
	const int worldHeight = 1300;

	const int circleCount = 1000;
	const int circleRadius = 16;

	SDL_Color circleColor{};

	glm::vec2* circlePos;
	glm::vec2* circleVel;
	std::queue<glm::vec2> explosions;

	Environment();
	~Environment();
};

