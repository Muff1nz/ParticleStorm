#include "SessionManager.h"
#include <vec2.hpp>
#include <iostream>
#include "Environment.h"
#include "Timer.h"
#include <fstream>
#include "Utils.h"
#include <windows.h>
#include "PhysicsEngine.h"
#include "RenderEngineVulkan.h"


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

	statsFile << "Title: " + longTitle + "\n";
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
	RenderEngineVulkan renderEngine(&environment, &stats);
	PhysicsEngine physicsEngine(&environment, &stats);
	if (renderEngine.Init()) {
		physicsEngine.Init();

		bool done = false;

		renderEngine.Start(&done);
		physicsEngine.Start(&done);

		std::vector<std::string> perSecondStats;

		Timer timer;
		timer.Start();

		int lastMouseButtonState = GLFW_PRESS;

		while (!done) {
			std::this_thread::sleep_for(std::chrono::microseconds(10));

			if (timer.ElapsedSeconds() >= 1) {
				timer.Restart();
				stats.CompleteLastSecond();
				std::cout << stats.LastSecondToStringConsole();
				perSecondStats.push_back(stats.LastSecondToString());
			}

			glfwPollEvents();
			if (glfwWindowShouldClose(renderEngine.GetWindow()))
				done = true;

			auto state = glfwGetMouseButton(renderEngine.GetWindow(), GLFW_MOUSE_BUTTON_LEFT);
			if (state == GLFW_PRESS && state != lastMouseButtonState) {
				double x, y;
				glfwGetCursorPos(renderEngine.GetWindow(), &x, &y);
				environment.explosions.push(glm::vec2(float(x), environment.worldHeight - float(y)));
				++stats.explosionTotalLastSecond;
			}
			lastMouseButtonState = state;
		}

		physicsEngine.Join();
		renderEngine.Join();

		stats.CompleteSession();
		std::cout << stats.CompleteSessionToStringConsole();

		OutputStatsToFile(stats, perSecondStats, environment);
	}

	renderEngine.Dispose();
}


void SessionManager::Benchmark() const {
	Timer::unhinged = true;

	Environment environment(20000, 5, 1337);
	Stats stats{};
	RenderEngineVulkan renderEngine(&environment, &stats);
	PhysicsEngine physicsEngine(&environment, &stats);
	if (renderEngine.Init()) {
		physicsEngine.Init();

		bool done = false;

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

		const int benchmarkDuration = 21;

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

			glfwPollEvents();
		}
		done = true;
		physicsEngine.Join();
		renderEngine.Join();

		stats.CompleteSession();
		std::cout << stats.CompleteSessionToStringConsole();

		OutputStatsToFile(stats, perSecondStats, environment);
	}

	renderEngine.Dispose();

	Timer::unhinged = false;
}
