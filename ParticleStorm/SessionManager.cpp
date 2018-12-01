#include "SessionManager.h"
#include <SDL2/SDL.h>
#include <vec2.hpp>
#include <iostream>
#include "Environment.h"
#include "Timer.h"
#include <fstream>
#include "Utils.h"
#include <windows.h>
#include "RenderEngineSDL.h"
#include "PhysicsEngine.h"


SessionManager::SessionManager() = default;


SessionManager::~SessionManager() = default;


char* SessionManager::FileTime() {
	char name[20];
	time_t now = time(0);
	struct tm buf;
	localtime_s(&buf, &now);
	strftime(name, sizeof(name), "%d%m%Y_%H%M%S", &buf);
	return name;
}

void SessionManager::OutputStatsToFile(const Stats& stats, const std::vector<std::string>& perSecondStats, const Environment& environment) const {
	const std::string fileLeadText = "PS_Stats_";
	const std::string statsOutputFolder = statsOutputDir + fileLeadText + shorTitle + "_" + FileTime();
	const std::string statsOutputFilePath = statsOutputFolder + "/" + fileLeadText + shorTitle + "_" + FileTime() + ".txt";

	std::cout << "Results are saved to: " + statsOutputFolder + "\n";
	CreateDirectory(statsOutputFolder.c_str(), nullptr);
	std::ofstream statsFile(statsOutputFilePath);

	statsFile << longTitle + "\n";
	statsFile << "Simulated " + std::to_string(environment.circleCount) + " particles with a raidus of: " + std::to_string(environment.circleRadius) + "\n";
	statsFile << "Quadtree max particles per quad: " + std::to_string(environment.tree->maxParticles) + "\n";
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

	auto command = "python \"" + statsGrapherDir + "\" \"" + statsOutputFilePath + "\"";
	std::cout << command + "\n";
	system(command.c_str());
}

void SessionManager::Sandbox() const {
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


void SessionManager::Benchmark() const {
	Environment environment(4000, 8, 1337);
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
		for (int i = 0; i < explosionPointCount; ++i) {
			explosionPoints[i] = { environment.worldWidth * (float(i) / (explosionPointCount - 1)) , environment.worldHeight };
		}

		Timer sessionTimer;
		sessionTimer.Start();

		Timer timer;
		timer.Start();

		Timer explosionTimer;
		explosionTimer.Start();

		const int benchmarkDuration = 11;

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
