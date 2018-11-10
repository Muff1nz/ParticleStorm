//TODO: Split this program into 3 components: PhysicsEngine, RenderEngine and Environment
//TODO: The first two are obvious, Environment will be a static class containing common data, mostly the circles(Particles)
//TODO: Move window and render data out of environment and into RenderEngine
//
//TODO: The first iteration of the RenderEngine will still be CPU-SDL based, which is slow, will write it again in Vulkan after 
//TODO:		the PhysicsEngine can handle particles colliding with each other (and possibly multi-threaded collisions with quad-trees for max performance) (But rendering is the bottleneck now anyways(by far))
//
//For now, keep messing it up in main for fun =)

#include <SDL2/SDL.h>
#include <string> 
#include "Environment.h"
#include "SDL_Circles.h"
#include <iostream>
#include <thread>

//=============Environment=============
//=============Environment=============
//=============Environment=============
//=============Environment=============
#define SCREEN_WIDTH 2400
#define SCREEN_HEIGHT 1300

const glm::vec2 gravity = glm::vec2(0, -0.001);
const float friction = 0.995f;
const int circleCount = 1000;
const int circleRadius = 16;
SDL_Color circleColor = SDL_Color();

glm::vec2 circlePos[circleCount];
glm::vec2 circleVel[circleCount];

//=============Physics Engine=============
//=============Physics Engine=============
//=============Physics Engine=============
//=============Physics Engine=============
const int maxPhysicsUpdatesPerSecond = 3000000;

void BoundingBoxCollision(int particle){
	if (circlePos[particle].y < circleRadius) {
		circleVel[particle].y = -circleVel[particle].y * friction;
		circlePos[particle].y = circleRadius;
	}
	if (circlePos[particle].y > SCREEN_HEIGHT - circleRadius) {
		circleVel[particle].y = -circleVel[particle].y * friction;
		circlePos[particle].y = SCREEN_HEIGHT - circleRadius;
	}
	if (circlePos[particle].x < circleRadius) {
		circleVel[particle].x = -circleVel[particle].x * friction;
		circlePos[particle].x = circleRadius;
	}
	if (circlePos[particle].x > SCREEN_WIDTH - circleRadius) {
		circleVel[particle].x = -circleVel[particle].x * friction;
		circlePos[particle].x = SCREEN_WIDTH - circleRadius;
	}
}

void ParticleCollision(int particle, float deltaTime) {

	for (int i = particle + 1; i < circleCount; i++) {
		float distance = glm::distance(circlePos[particle], circlePos[i]);
		if (distance < circleRadius * 2) {
			glm::vec2 positionDelta = circlePos[particle] - circlePos[i];
			
			glm::vec2 newVel1 = circleVel[particle] - glm::dot(circleVel[particle] - circleVel[i], positionDelta) / pow(glm::length(positionDelta), 2) * positionDelta;

			positionDelta = -positionDelta;
			glm::vec2 newVel2 = circleVel[i] - glm::dot(circleVel[i] - circleVel[particle], positionDelta) / pow(glm::length(positionDelta), 2) * positionDelta;

			circleVel[particle] = newVel1 * friction;
			circleVel[i] = newVel2 * friction;

			float overlap = circleRadius * 2 - distance;

			circlePos[particle] += -(positionDelta / distance) * (overlap / 2);
			circlePos[i] += positionDelta / distance * (overlap / 2);
		}		
	}
}

void physicsThreadRun(SDL_bool* done, int* physicsUpdates){
	Uint64 NOW = SDL_GetPerformanceCounter();
	Uint64 LAST = NOW;
	float deltaTime;

	while (!(*done)) {
		NOW = SDL_GetPerformanceCounter();
		deltaTime = float((NOW - LAST) * 1000 / float(SDL_GetPerformanceFrequency()));
		if (deltaTime > 1000 / maxPhysicsUpdatesPerSecond) {
			LAST = NOW;

			for (int i = 0; i < circleCount; i++) {
				circleVel[i] += gravity * deltaTime;
				circlePos[i] += circleVel[i] * deltaTime;
				BoundingBoxCollision(i);
			}

			for (int i = 0; i < circleCount; i++) {
				ParticleCollision(i, deltaTime);
			}
			(*physicsUpdates)++;
		}
	}
}

//=============Render Engine=============
//=============Render Engine=============
//=============Render Engine=============
//=============Render Engine=============
void renderThreadRun(SDL_bool* done, int* renderUpdates) {
		while (!(*done)) {
		SDL_SetRenderDrawColor(Environment::renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(Environment::renderer);
		SDL_SetRenderDrawColor(Environment::renderer, 0, 140, 0, SDL_ALPHA_OPAQUE);
		for (auto circlePo : circlePos) {
			SDL_Circles::render_circle(circlePo, circleRadius, circleRadius);
		}
		SDL_RenderPresent(Environment::renderer);
		(*renderUpdates)++;
	}
}

int main(int argc, char* args[]) {
	
	if (SDL_Init(SDL_INIT_VIDEO) == 0) {

		if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &Environment::window, &Environment::renderer) == 0) {
			SDL_bool done = SDL_FALSE;

			Uint64 NOW = SDL_GetPerformanceCounter();
			Uint64 LAST;
			double deltaTime;

			circleColor.g = 140;
			circleColor.a = 255;
			const int maxSpeed = 1;
			for (int i = 0; i < circleCount; i++) {
				circlePos[i] = glm::vec2(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);

				do {
					circleVel[i] = glm::vec2(rand() % maxSpeed * 2 - maxSpeed, rand() % maxSpeed * 2 - maxSpeed);
				} while (abs(circleVel[i].x) < 1 && abs(circleVel[i].y) < 1);
			}			

			int renderUpdates = 0;
			int physicsUpdates = 0;
			double timer = 0;
			SDL_SetRenderDrawColor(Environment::renderer, 0, 140, 0, SDL_ALPHA_OPAQUE);
			std::thread physicsThread(physicsThreadRun, &done, &physicsUpdates);
			std::thread renderThread(renderThreadRun, &done, &renderUpdates);

			while (!done) {
				SDL_Event event;

				LAST = NOW;
				NOW = SDL_GetPerformanceCounter();
				deltaTime = (double)((NOW - LAST) * 1000 / double(SDL_GetPerformanceFrequency()));
				timer += deltaTime;

				if (timer > 1000) {
					timer = 0;
					std::cout << "==================================\n";
					std::cout << "Simulating : " + std::to_string(circleCount) + " particles!\n";
					std::cout << "Physics updates last second: " + std::to_string(physicsUpdates) + "\n";
					std::cout << "Render updates last second: " + std::to_string(renderUpdates) + "\n";
					physicsUpdates = 0;
					renderUpdates = 0;
				}

				while (SDL_PollEvent(&event)) {
					if (event.type == SDL_QUIT) {
						done = SDL_TRUE;
					}
					LAST = SDL_GetPerformanceCounter();
				}
			}
			physicsThread.join();
			renderThread.join();
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


