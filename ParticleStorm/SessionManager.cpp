#include "SessionManager.h"
#include <SDL2/SDL.h>
#include <vec2.hpp>
#include <iostream>
#include "Environment.h"
#include "RenderEngineSDL.h"
#include "PhysicsEngine.h"
#include "Timer.h"
#include <fstream>
#include "Utils.h"


SessionManager::SessionManager() {
}


SessionManager::~SessionManager() {
}


const std::string shorTitle = "Benchmark";
const std::string longTitle = "Currently making benchmark for ParticleStorm";

char* SessionManager::FileTime() {
	char name[20];
	time_t now = time(0);
	struct tm buf;
	localtime_s(&buf, &now);
	strftime(name, sizeof(name), "%d%m%Y_%H%M%S", &buf);
	return name;
}

void SessionManager::OutputStatsToFile(const Stats& stats, const std::vector<std::string>& perSecondStats, const Environment& environment) {
	const std::string statsOutputPath = "C:/C++ Projects/ParticleStorm_Stats/PS_Stats_";
	std::cout << statsOutputPath + FileTime() + ".txt";
	std::ofstream statsFile(statsOutputPath + shorTitle + "_" + FileTime() + ".txt");
	statsFile << longTitle + "\n";
	statsFile << "Simulated " + std::to_string(environment.circleCount) + " particles with a raidus of: " + std::to_string(environment.circleRadius) + "\n";
	statsFile << "Duration: " + std::to_string(perSecondStats.size()) + " seconds\n";
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

void SessionManager::Sandbox() {
	Environment environment{};
	Stats stats{};
	RenderEngineSDL renderEngine(&environment, &stats);
	PhysicsEngine physicsEngine(&environment, &stats);
	if (renderEngine.Init()) {
		physicsEngine.Init();

		SDL_bool done = SDL_FALSE;

		renderEngine.Start(&done);
		physicsEngine.Start(&done);

		std::vector<std::string> perSecondStats;

		Timer timer;
		timer.Start();

		while (!done) {
			SDL_Event event;

			std::this_thread::sleep_for(std::chrono::microseconds(10));

			if (timer.ElapsedSeconds() >= 1) {
				timer.Restart();
				stats.CompleteLastSecond();
				std::cout << stats.LastSecondToStringConsole();
				perSecondStats.push_back(stats.LastSecondToString());
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
}


void SessionManager::Benchmark() {
	Environment environment(4000, 8);
	Stats stats{};
	RenderEngineSDL renderEngine(&environment, &stats);
	PhysicsEngine physicsEngine(&environment, &stats);
	if (renderEngine.Init()) {
		physicsEngine.Init();

		SDL_bool done = SDL_FALSE;

		renderEngine.Start(&done);
		physicsEngine.Start(&done);

		std::vector<std::string> perSecondStats;

		int explosionIndex = 0;
		const int explosionPointCount = 4;
		glm::vec2 explosionPoints[explosionPointCount];
		
		std::cout << "\n";
		for (int i = 0; i < explosionPointCount; ++i) {
			explosionPoints[i] = { environment.worldWidth * (float(i) / (explosionPointCount - 1)) , environment.worldHeight };
			std::cout << Utils::vecToString(explosionPoints[i]) + " ";
		}
		std::cout << "\n";

		Timer sessionTimer;
		sessionTimer.Start();

		Timer timer;
		timer.Start();

		Timer explosionTimer;
		explosionTimer.Start();

		const int benchmarkDuration = 10;

		while (sessionTimer.ElapsedSeconds() <= benchmarkDuration) {
			std::this_thread::sleep_for(std::chrono::microseconds(10));

			if (timer.ElapsedSeconds() >= 1) {
				timer.Restart();
				stats.CompleteLastSecond();
				std::cout << stats.LastSecondToStringConsole();
				perSecondStats.push_back(stats.LastSecondToString());

				std::cout << "\nBenchmark is " + std::to_string(sessionTimer.ElapsedSeconds() / float(benchmarkDuration) * 100) + "% complete\n";
			}

			if (explosionTimer.ElapsedSeconds() >= 1.0f) {
				explosionTimer.Restart();
				const auto impact = explosionPoints[explosionIndex++ % explosionPointCount];
				environment.explosions.push(glm::vec2(impact.x, environment.worldHeight - impact.y));
			}
		}
		done = SDL_TRUE;
		physicsEngine.Join();
		renderEngine.Join();

		stats.CompleteSession();
		std::cout << stats.CompleteSessionToStringConsole();

		OutputStatsToFile(stats, perSecondStats, environment);
	}

	renderEngine.Dispose();

	SDL_Quit();
}