#pragma once
#include <SDL2/SDL.h>
#include "Environment.h"
#include <thread>
#include "QuadTree.h"
#include <vec3.hpp>
#include "Stats.h"

class RenderEngineSDL {
public:
	RenderEngineSDL(Environment* environment, Stats* stats);
	~RenderEngineSDL();

	bool Init();
	void Start(SDL_bool* done);
	void Join();
	void Dispose() const;
private:
	Environment* environment;
	Stats* stats;

	SDL_Renderer* renderer{};
	SDL_Window* window{};

	glm::vec2* particlesRenderCopy;

	std::thread renderThead;

	void RenderCircle(glm::vec2 center, float radius, unsigned sides) const;
	void RenderLine(glm::vec2 start, glm::vec2 end) const;
	void RenderLine(int x1, int y1, int x2, int y2) const;
	void SetRenderColor(const SDL_Color& color) const;
	void GetRenderColor(SDL_Color& oldColor) const;
	void RenderThreadRun(const SDL_bool* done) const;
	void RenderQuadTree(const QuadTree* tree) const;
};

