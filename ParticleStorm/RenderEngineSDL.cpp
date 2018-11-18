#include "RenderEngineSDL.h"
#include "Environment.h"
#include "SDL_Circles.h"

#define _2PI 6.28f

RenderEngineSDL::RenderEngineSDL(Environment* environment) {
	this->environment = environment;
}

RenderEngineSDL::~RenderEngineSDL() = default;

bool RenderEngineSDL::Init() {
	return
		SDL_Init(SDL_INIT_VIDEO) == 0 &&
		SDL_CreateWindowAndRenderer(
			environment->worldWidth,
			environment->worldHeight,
			0,
			&window,
			&renderer) == 0;
}

void RenderEngineSDL::Start(SDL_bool* done, int* renderUpdates) {
	renderThead = std::thread([=] {RenderThreadRun(done, renderUpdates);});
}

void RenderEngineSDL::Join() {
	renderThead.join();
}

void RenderEngineSDL::Dispose() const {
	if (renderer) {
		SDL_DestroyRenderer(renderer);
	}
	if (window) 
		SDL_DestroyWindow(window);
}

/**
* Me stealy wheely https://forums.libsdl.org/viewtopic.php?p=46027
**/
void RenderEngineSDL::RenderCircle(const glm::vec2 center, const float radius, unsigned int sides) const {
	if (sides == 0) {
		sides = _2PI*radius / 2;
	}

	const auto dA = _2PI / sides;
	auto angle = dA;

	glm::vec2 end;
	end.x = radius;
	end.y = 0.0f;
	end = end + center;
	for (int i = 0; i != sides; i++) {
		const auto start = end;
		end.x = cos(angle) * radius;
		end.y = sin(angle) * radius;
		end = end + center;
		angle += dA;
		RenderLine(start, end);
		RenderLine(center, end);
	}
}

void RenderEngineSDL::RenderLine(const glm::vec2 start, const glm::vec2 end) const {
	// Draw a line 
	//--- 
	int ret =
		SDL_RenderDrawLine(
			renderer, // SDL_Renderer* renderer: the renderer in which draw 
			start.x,               // int x1: x of the starting point 
			environment->worldHeight - start.y,          // int y1: y of the starting point 
			end.x,                 // int x2: x of the end point 
			environment->worldHeight - end.y);           // int y2: y of the end point 
}


void RenderEngineSDL::SetRenderColor(const SDL_Color& color) const {
	int ret = SDL_SetRenderDrawColor(renderer,
		color.r,
		color.g,
		color.b,
		color.a);
}

void RenderEngineSDL::GetRenderColor(SDL_Color& oldColor) const {
	SDL_GetRenderDrawColor(renderer,
		&oldColor.r,
		&oldColor.g,
		&oldColor.b,
		&oldColor.a);
}

void RenderEngineSDL::RenderThreadRun(const SDL_bool* done, int* renderUpdates) const {
	while (!(*done)) {
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 0, 140, 0, SDL_ALPHA_OPAQUE);
		const auto circles = environment->circlePos;
		for (int i = 0; i < environment->circleCount; ++i) {
			RenderCircle(circles[i], environment->circleRadius, environment->circleRadius);
		}
		SDL_RenderPresent(renderer);
		(*renderUpdates)++;
	}
}
