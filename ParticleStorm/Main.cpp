//TODO: Create vulkan render engine
//TODO:	Implement more then 1 thread and quad trees for physics engine
//
// BUG: "Particle Soup", when the particle density is high, particles fail to stack and fall into each other instead.
// BUG: Maybe playing with gravity and friction could help. Make Gravity weaker for stationary particles.

#include <SDL2/SDL.h>
#include <string> 
#include "Environment.h"
#include "SDL_Circles.h"
#include <iostream>
#include <thread>
#include <queue>
#include "RenderEngineSDL.h"
#include "PhysicsEngine.h"

int main(int argc, char* args[]) {
	Environment environment{};
	RenderEngineSDL renderEngine(&environment);
	PhysicsEngine physicsEngine(&environment);
	if (renderEngine.Init()) {
		physicsEngine.Init();

		SDL_bool done = SDL_FALSE;

		Uint64 NOW = SDL_GetPerformanceCounter();
		Uint64 LAST;

		int renderUpdates = 0;
		int physicsUpdates = 0;
		double timer = 0;

		renderEngine.Start(&done, &renderUpdates);
		physicsEngine.Start(&done, &physicsUpdates);

		while (!done) {
			SDL_Event event;

			std::this_thread::sleep_for(std::chrono::microseconds(10));

			LAST = NOW;
			NOW = SDL_GetPerformanceCounter();
			double deltaTime = (double)((NOW - LAST) * 1000 / double(SDL_GetPerformanceFrequency()));
			timer += deltaTime;

			if (timer > 1000) {
				timer = 0;
				std::cout << "==================================\n";
				std::cout << "Simulating : " + std::to_string(environment.circleCount) + " particles!\n";
				std::cout << "Physics updates last second: " + std::to_string(physicsUpdates) + "\n";
				std::cout << "Render updates last second: " + std::to_string(renderUpdates) + "\n";
				physicsUpdates = 0;
				renderUpdates = 0;
			}

			while (SDL_PollEvent(&event)) {
				switch (event.type) {
					case SDL_QUIT:
						done = SDL_TRUE;
						break;
					case SDL_MOUSEBUTTONDOWN:
						if (event.button.button == SDL_BUTTON_LEFT) {
							environment.explosions.push(glm::vec2(event.button.x, environment.worldHeight - event.button.y));
							std::cout << "BOOM! At x: " + std::to_string(event.button.x) + " y:" + std::to_string(environment.worldHeight - event.button.y) + "\n";
						}
						break;
				}
			}
		}
		physicsEngine.Join();
		renderEngine.Join();
	}

	renderEngine.Dispose();	
	
	SDL_Quit();
	return 0;
}


