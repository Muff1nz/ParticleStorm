#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <map>
#include <vector>

class EventEngine {
public:
	EventEngine();
	~EventEngine();

	void Init(GLFWwindow* window);

	bool GetKeyDown(int key);
	bool GetKey(int key);
	bool GetKeyUp(int key);

	bool GetMouseButtonDown(int mouseButton);
	bool GetMouseButton(int mouseButton);
	bool GetMouseButtonUp(int mouseButton);
	
	void Update();
private:	
	GLFWwindow* window;
	
	std::map<int, int> keyStatesGhost;
	std::map<int, int> keyStates;
	std::vector<int> keyHooks;

	std::map<int, int> mouseButtonStatesGhost;
	std::map<int, int> mouseButtonStates;
	std::vector<int> mouseButtonHooks;

	void HookKeyIfNeeded(int key);
	void HookMouseButtonIfNeeded(int mouseButton);
};

