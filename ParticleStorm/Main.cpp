//TODO: Create vulkan render engine
//TODO:	Implement more then 1 thread and quad trees for physics engine
//
// BUG: "Particle Soup", when the particle density is high, particles fail to stack and fall into each other instead.
// BUG: Maybe playing with gravity and friction could help. Make Gravity weaker for stationary particles.

#include <SDL2/SDL.h>
#include <string> 
#include "Environment.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <queue>
#include "RenderEngineSDL.h"
#include "PhysicsEngine.h"

const std::string shorTitle = "StatsClass";
const std::string longTitle = "Currently making stats class for ParticleStorm";

char* fileTime() {
	char name[20];
	time_t now = time(0);
	struct tm buf;
	localtime_s(&buf, &now);
	strftime(name, sizeof(name), "%d%m%Y_%H%M%S", &buf);
	return name;
}

void OutputStatsToFile(const Stats& stats, const std::vector<std::string>& perSecondStats, const Environment& environment){
	const std::string statsOutputPath = "C:/C++ Projects/ParticleStorm_Stats/PS_Stats_";
	std::cout << statsOutputPath + fileTime() + ".txt";
	std::ofstream statsFile(statsOutputPath + shorTitle + "_" + fileTime() + ".txt");
	statsFile << longTitle + "\n";
	statsFile << "Simulated " + std::to_string(environment.circleCount) + " particles with a raidus of: " + std::to_string(environment.circleRadius) + "\n";
	statsFile << "[\n";
	statsFile << stats.CompleteSessionToString();
	statsFile << "]\n";
	for (int i = 0; i < perSecondStats.size(); i++) {
		statsFile << "{\n";
		statsFile << std::to_string(i + 1) + "\n";
		statsFile << perSecondStats[i];
		statsFile << "}\n";
	}
	statsFile.close();
}

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


		std::vector<std::string> perSecondStats;


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
				perSecondStats.push_back(stats.LastSecondToString());
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
							++stats.explosionTotalLastSecond;
						}
						break;
				}
			}
		}
		physicsEngine.Join();
		renderEngine.Join();
		
		stats.CompleteSession();
		std::cout << stats.CompleteSessionToStringConsole();

		OutputStatsToFile(stats, perSecondStats, environment);
	}
	
	renderEngine.Dispose();	

	SDL_Quit();
	return 0;
}


