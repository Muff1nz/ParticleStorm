#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <map>
#include <vector>
#include "Queue.h"
#include <vec2.hpp>
#include "MessageSystem.h"

class EventEngine {
public:
	EventEngine(MessageSystem* messageQueue);
	~EventEngine();

	void Init(GLFWwindow* window);

	bool GetKeyDown(int key);
	bool GetKey(int key);
	bool GetKeyUp(int key);

	bool GetMouseButtonDown(int mouseButton);
	bool GetMouseButton(int mouseButton);
	bool GetMouseButtonUp(int mouseButton);

	glm::vec2 GetMousePos() const;

	int GetMouseScrollDelta() const;	
	
	void Update();
private:
	MessageSystem* messageQueue;

	GLFWwindow* window;
	
	std::map<int, int> keyStatesGhost;
	std::map<int, int> keyStates;
	std::vector<int> keyHooks;

	std::map<int, int> mouseButtonStatesGhost;
	std::map<int, int> mouseButtonStates;
	std::vector<int> mouseButtonHooks;

	glm::vec2 mousePos;

	int mouseScrollDelta;
	static Queue<double> scrollEvents;

	void HookKeyIfNeeded(int key);
	void HookMouseButtonIfNeeded(int mouseButton);
	void static ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
};
