#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Window {
public:
	void InitWindow(int height, int width, bool fullscreen);
	void Dispose();

	GLFWwindow* GetWindow() const;
	
	int GetHeight() const;
	int GetWidth() const;

	bool IsFullscreen() const;
	bool IsMinimized() const;

	
	void UpdateMetaData();
	void ToggleFullscreen();
	
private:
	bool isDisposed = false;
	
	GLFWwindow* window{};
	GLFWmonitor* monitor = nullptr;

	bool fullscreen = false;

	int refreshRate = 0;

	int screenHeight = 0;
	int screenWidth = 0;

	int height = 0;
	int width = 0;

	int posX = 0;
	int posy = 0;
};