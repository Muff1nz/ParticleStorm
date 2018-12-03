#pragma once
#include <vec3.hpp>
#include <vec2.hpp>
#include <thread>
#include "Stats.h"
#include "Environment.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class QuadTree;

class RenderEngineVulkan {
public:
	RenderEngineVulkan(Environment* environment, Stats* stats);
	~RenderEngineVulkan();

	void Init();
	void Start(bool* done);
	void Join();
	void Dispose() const;

	GLFWwindow* GetWindow();
private:
	Environment* environment;
	Stats* stats;

	GLFWwindow* window{};
	VkInstance instance;

	glm::vec2* particlesRenderCopy;

	std::thread renderThead;

	void InitWindow();
	void CreateInstance();
	void InitVulkan();
	void RenderThreadRun(bool* done) const;
};

