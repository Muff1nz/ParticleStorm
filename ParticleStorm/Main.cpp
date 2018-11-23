//TODO: Create vulkan render engine
//TODO:	Implement more then 1 thread and quad trees for physics engine
//
// BUG: "Particle Soup", when the particle density is high, particles fail to stack and fall into each other instead.
// BUG: Maybe playing with gravity and friction could help. Make Gravity weaker for stationary particles.

#include <SDL2/SDL.h>
#include <string> 
#include "Environment.h"
#include <iostream>
#include <thread>
#include <queue>
#include "RenderEngineSDL.h"
#include "PhysicsEngine.h"

int main(int argc, char* args[]) {
	Environment environment{};
	Stats stats{};
	RenderEngineSDL renderEngine(&environment, &stats);
	PhysicsEngine physicsEngine(&environment, &stats);
	if (renderEngine.Init()) {
		physicsEngine.Init();

		SDL_bool done = SDL_FALSE;

		Uint64 NOW = SDL_GetPerformanceCounter();
		Uint64 LAST;


		double timer = 0;

		renderEngine.Start(&done);
		physicsEngine.Start(&done);

		while (!done) {
			SDL_Event event;

			std::this_thread::sleep_for(std::chrono::microseconds(10));

			LAST = NOW;
			NOW = SDL_GetPerformanceCounter();
			double deltaTime = (double)((NOW - LAST) * 1000 / double(SDL_GetPerformanceFrequency()));
			timer += deltaTime;

			if (timer > 1000 ) {
				stats.CompleteLastSecond();
				std::cout << stats.LastSecondToStringConsole();
				timer = 0;
			}


			while (SDL_PollEvent(&event)) {
				switch (event.type) {
					case SDL_QUIT:
						done = SDL_TRUE;
						break;
					case SDL_MOUSEBUTTONDOWN:
						if (event.button.button == SDL_BUTTON_LEFT) {
							environment.explosions.push(glm::vec2(event.button.x, environment.worldHeight - event.button.y));
						}
						break;
				}
			}
		}
		physicsEngine.Join();
		renderEngine.Join();
		
		stats.CompleteSession();
		std::cout << stats.CompleteSessionToStringConsole();
	}

	renderEngine.Dispose();	
	
	SDL_Quit();
	return 0;
}


