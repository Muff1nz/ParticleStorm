#pragma once
#include <vec2.hpp>
#include <mat4x4.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Queue.h"
#include "Window.h"

class Camera {
public:
	Camera();
	Camera(int worldHeight, int worldWidth, Window* window);
	~Camera();
	void Init(GLFWwindow* window);

	glm::mat4 GetView();
	glm::mat4 GetProj();

	glm::vec2 GetWorldPos(glm::vec2 screenPos);
	glm::vec2 GetViewPos(glm::vec2 screenPos);

	void Update(GLFWwindow* window, float deltaTime);

private:	
	int worldHeight;
	int worldWidth;
	Window* window;


	static Queue<double> scrollEvents;

	glm::vec2 pos;

	float zoom;
	float minZoom = -0.49f;
	float maxZoom = 10.0f;

	glm::vec2 oldMousePos;
	glm::vec2 oldMouseFlag = { -999, -999 };

	float GetPixelToWorld() const;
	int GetOrthoHeight() const;
	int GetOrthoWidth() const;
	void ResetCamera();
	void static ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
};
