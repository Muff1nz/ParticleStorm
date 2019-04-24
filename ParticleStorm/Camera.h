#pragma once
#include <vec2.hpp>
#include <mat4x4.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Camera {
public:
	Camera();
	Camera(int worldHeight, int worldWidth, int screenHeight, int screenWidth);
	void Init(GLFWwindow* window);

	glm::mat4 GetView();
	glm::mat4 GetProj();

	glm::vec2 GetWorldPos(glm::vec2 screenPos);
	glm::vec2 GetViewPos(glm::vec2 screenPos);
	void Update(GLFWwindow* window, float deltaTime);

private:	
	int worldHeight;
	int worldWidth;
	int screenHeight;
	int screenWidth;

	float cameraSpeed = 500.0f;
	float cameraZoomSpeed = 5.0f;

	glm::vec2 pos;
	float zoom;
	glm::vec2 oldMousePos;
	glm::vec2 oldMouseFlag = { -999, -999 };

	void ProcessKeyboardInput(GLFWwindow* getWindow, float deltaTime);
	void ProcessMouseInput(GLFWwindow* getWindow, float deltaTime);
	void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
};
