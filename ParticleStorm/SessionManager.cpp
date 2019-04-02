#include "SessionManager.h"

#include <iostream>
#include <fstream>
#include <windows.h>

#include "Timer.h"
#include "Environment.h"
#include "PhysicsEngine.h"
#include "RenderEngineVulkan.h"
#include "Utils.h"


SessionManager::SessionManager() = default;


SessionManager::~SessionManager() = default;


void SessionManager::ControlCamera(Environment& environment, GLFWwindow* getWindow, float deltaTime) const {
	const float cameraSpeed = 500.0f;
	float cameraZoomSpeed = environment.camera.zoom <= 1 ? 0.5f : 5.0f;

	if (glfwGetKey(getWindow, GLFW_KEY_A) == GLFW_PRESS) {
		environment.camera.pos.x -= cameraSpeed * deltaTime;
	}

	if (glfwGetKey(getWindow, GLFW_KEY_D) == GLFW_PRESS) {
		environment.camera.pos.x += cameraSpeed * deltaTime;
	}

	if (glfwGetKey(getWindow, GLFW_KEY_W) == GLFW_PRESS) {
		environment.camera.pos.y -= cameraSpeed * deltaTime;
	}

	if (glfwGetKey(getWindow, GLFW_KEY_S) == GLFW_PRESS) {
		environment.camera.pos.y += cameraSpeed * deltaTime;
	}

	if (glfwGetKey(getWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		environment.camera.zoom -= cameraZoomSpeed * deltaTime;
	}

	if (glfwGetKey(getWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		environment.camera.zoom += cameraZoomSpeed * deltaTime;
	}

	if (environment.camera.zoom <= -0.49f)
		environment.camera.zoom = -0.49f;
}


char* SessionManager::FileTime() {
	char name[20];
	time_t now = time(0);
	struct tm buf;
	localtime_s(&buf, &now);
	strftime(name, sizeof(name), "%d%m%Y_%H%M%S", &buf);
	return name;
}

void SessionManager::OutputMultiRunToFile(const std::string& sessionString) const {
	const std::string fileLeadText = "Benchmark_";
	const std::string statsOutputFolder = statsOutputDir + fileLeadText + shorTitle + "_" + FileTime();
	const std::string statsOutputFilePath = statsOutputFolder + "/" + fileLeadText + shorTitle + "_" + FileTime() + ".txt";

	std::cout << "Results are saved to: " + statsOutputFolder + "\n";
	CreateDirectory(statsOutputFolder.c_str(), nullptr);
	std::ofstream statsFile(statsOutputFilePath);

	statsFile << sessionString;

	statsFile.close();

	auto command = "python \"" + multiStatsGrapherDir + "\" \"" + statsOutputFilePath + "\"";
	std::cout << command + "\n";
	system(command.c_str());

	command = "python \"" + singleStatsGrapherDir + "\" \"" + statsOutputFilePath + "\"";
	std::cout << command + "\n";
	system(command.c_str());

	command = "python \"" + physicsDetailedGrapherDir + "\" \"" + statsOutputFilePath + "\"";
	std::cout << command + "\n";
	system(command.c_str());
}

std::string SessionManager::SessionToString(const std::vector<std::string>& perSecondStats, const Environment& environment) const {
	std::string sessionString;
	sessionString += "Title: " + longTitle + "\n";
	sessionString += "Simulated " + std::to_string(environment.particleCount) + " particles with a raidus of: " + std::to_string(environment.particleRadius) + "\n";
	sessionString += "Worker threads: " + std::to_string(environment.workerThreadCount) + "\n";
	sessionString += "Duration: " + std::to_string(perSecondStats.size()) + " seconds\n";
	sessionString += "[\n";
	sessionString += environment.stats.CompleteSessionToString();
	sessionString += "]\n";
	for (int i = 0; i < perSecondStats.size(); i++) {
		sessionString += "{\n";
		sessionString += std::to_string(i + 1) + "\n";
		sessionString += perSecondStats[i];
		sessionString += "}\n";
	}
	return sessionString;
}

void SessionManager::Sandbox() const {
	Environment environment{};
	RenderEngineVulkan renderEngine(&environment);
	PhysicsEngine physicsEngine(&environment);
	renderEngine.Init();
	physicsEngine.Init();

	renderEngine.Start();
	physicsEngine.Start();

	Timer timer;
	timer.Start();

	int lastMouseButtonState = GLFW_PRESS;
	float deltaTime;

	while (!environment.done) {
		std::this_thread::sleep_for(std::chrono::microseconds(10));

		deltaTime = timer.DeltaTime();
		
		if (timer.ElapsedSeconds() >= 1) {
			timer.Restart();
			environment.stats.CompleteLastSecond();
			std::cout << environment.stats.LastSecondToStringConsole();
		}

		glfwPollEvents();
		if (glfwWindowShouldClose(renderEngine.GetWindow()))
			environment.done = true;

		auto state = glfwGetMouseButton(renderEngine.GetWindow(), GLFW_MOUSE_BUTTON_LEFT);
		if (state == GLFW_PRESS && state != lastMouseButtonState) {
			double x, y;
			glfwGetCursorPos(renderEngine.GetWindow(), &x, &y);

			glm::vec2 mouseWorldPos = environment.camera.GetWorldPos({ x, y });
			std::cout << "PixelPos: " << Utils::VecToString({ x, y }) << " WorldPos: " << Utils::VecToString({ mouseWorldPos.x, mouseWorldPos.y }) << "\n\n";			
			environment.explosions.push(glm::vec2(mouseWorldPos.x, mouseWorldPos.y));
			++environment.stats.explosionTotalLastSecond;
		}
		lastMouseButtonState = state;

		ControlCamera(environment, renderEngine.GetWindow(), deltaTime);
	}

	physicsEngine.Join();
	renderEngine.Join();

	environment.stats.CompleteSession();
	std::cout << environment.stats.CompleteSessionToStringConsole();

	renderEngine.Dispose();
}


std::string SessionManager::Benchmark(int particleCount, int particleRadius, int threadCount) const {
	Timer::unhinged = true;

	Environment environment(particleCount, particleRadius, 1337, threadCount);
	RenderEngineVulkan renderEngine(&environment);
	PhysicsEngine physicsEngine(&environment);
	renderEngine.Init();
	physicsEngine.Init();

	renderEngine.Start();
	physicsEngine.Start();

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

	const int benchmarkDuration = 10;

	while (perSecondStats.size() < benchmarkDuration) {
		std::this_thread::sleep_for(std::chrono::microseconds(10));

		if (timer.ElapsedSeconds() >= 1) {
			timer.Restart();
			environment.stats.CompleteLastSecond();
			std::cout << environment.stats.LastSecondToStringConsole();
			perSecondStats.push_back(environment.stats.LastSecondToString());

			std::cout << "\nBenchmark is " + std::to_string(perSecondStats.size() / float(benchmarkDuration) * 100) + "% complete\n";
		}

		if (explosionTimer.ElapsedSeconds() >= 1.0f) {
			explosionTimer.Restart();
			const auto impact = explosionPoints[explosionIndex++ % explosionPointCount];
			environment.explosions.push(glm::vec2(impact.x, environment.worldHeight - impact.y));
		}

		glfwPollEvents();
	}
	environment.done = true;

	environment.stats.CompleteSession();
	std::cout << environment.stats.CompleteSessionToStringConsole();

	physicsEngine.Join();
	renderEngine.Join();

	renderEngine.Dispose();

	Timer::unhinged = false;

	return SessionToString(perSecondStats, environment);
}

void SessionManager::Benchmark() const {
	const int threadRuns = 4;
	const int particleRuns = 3;
	int threadCounts[] = { 4, 8, 16, 30 };
	int particleCounts[] = { 20000, 40000, 80000 };
	int particleRadiuses[] = { 4, 3, 2 };

	std::string sessionString = "";

	for (int i = 0; i < particleRuns; ++i) {
		for (int j = 0; j < threadRuns; ++j) {
			sessionString += "<\n";
			if (j == threadRuns - 1 && i == particleRuns - 1)
				sessionString += "(\n";
			sessionString += Benchmark(particleCounts[i], particleRadiuses[i], threadCounts[j]);
			if (j == threadRuns - 1 && i == particleRuns - 1)
				sessionString += ")\n";
			sessionString += ">\n";
		}
	}
	OutputMultiRunToFile(sessionString);
}
