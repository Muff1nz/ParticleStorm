#pragma once
#include <SDL2/SDL.h>

class Environment {
public:
	Environment();
	~Environment();
	static SDL_Renderer* renderer;
	static SDL_Window* window;
};

