#pragma once
#include <SDL2/SDL.h>
#include "Environment.h"
#include <thread>

class RenderEngineSDL {
public:
	RenderEngineSDL(Environment* environment);
	~RenderEngineSDL();

	bool Init();
	void Start(SDL_bool* done, int* renderUpdates);
	void Join();
	void Dispose() const;
private:
	Environment* environment;

	SDL_Renderer* renderer;
	SDL_Window* window;

	std::thread renderThead;

	void RenderCircle(glm::vec2 center, float radius, unsigned sides) const;
	void RenderLine(glm::vec2 start, glm::vec2 end) const;
	void SetRenderColor(const SDL_Color& color) const;
	void GetRenderColor(SDL_Color& oldColor) const;
	void RenderThreadRun(const SDL_bool* done, int* renderUpdates) const;
};

