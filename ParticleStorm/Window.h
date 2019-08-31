#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Window {
public:
	void InitWindow(int height, int width, bool fullscreen);
	void Dispose();

	int GetHeight();
	int GetWidth();
	int IsFullscreen();
	GLFWwindow* GetWindow();
private:
	bool isDisposed = false;
	
	GLFWwindow* window{};
	int height = 0;
	int width = 0;
	bool fullscreen = false;	
};