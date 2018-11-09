// SDL2 Hello, World!
// This should display a white screen for 2 seconds
// compile with: clang++ main.cpp -o hello_sdl2 -lSDL2
// run with: ./hello_sdl2
/**
* Me stealy wheely https://gist.github.com/fschr/92958222e35a823e738bb181fe045274
**/
#include <SDL2/SDL.h>
#include <stdio.h>
#include "Environment.h"
#include "SDL_Circles.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

int main(int argc, char* args[]) {
	
	if (SDL_Init(SDL_INIT_VIDEO) == 0) {

		if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &Environment::window, &Environment::renderer) == 0) {
			SDL_bool done = SDL_FALSE;

			SDL_Color circleColor = SDL_Color();
			circleColor.g = 140;
			circleColor.a = 255;
			glm::vec2 circelPos = glm::vec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
			glm::vec2 circelVel = glm::vec2(0, 0);
			glm::vec2 gravity = glm::vec2(0, -0.01f);

			Uint64 NOW = SDL_GetPerformanceCounter();
			Uint64 LAST;
			double deltaTime = 0;


			while (!done) {
				SDL_Event event;

				LAST = NOW;
				NOW = SDL_GetPerformanceCounter();
				deltaTime = (double)((NOW - LAST) * 1000 / double(SDL_GetPerformanceFrequency()));

				//Physics
				circelVel += gravity * float(deltaTime);
				circelPos += circelVel * float(deltaTime);
				if (circelPos.y < 64) {
					circelVel = -circelVel;
					circelPos.y = 64;
				}

				//Rendering
				SDL_SetRenderDrawColor(Environment::renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
				SDL_RenderClear(Environment::renderer);

				SDL_Circles::render_circle(circelPos, 64, circleColor, 64);
				SDL_RenderPresent(Environment::renderer);

				while (SDL_PollEvent(&event)) {
					if (event.type == SDL_QUIT) {
						done = SDL_TRUE;
					}
				}
			}
		}

		if (Environment::renderer) {
			SDL_DestroyRenderer(Environment::renderer);
		}
		if (Environment::window) {
			SDL_DestroyWindow(Environment::window);
		}
	}
	SDL_Quit();
	return 0;
}


