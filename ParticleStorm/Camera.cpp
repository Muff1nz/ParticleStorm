#include "Camera.h"
#include <ext/matrix_transform.inl>
#include <ext/matrix_clip_space.inl>
#include <iostream>
#include "Utils.h"

Queue<double> Camera::scrollEvents{};

Camera::Camera() {
	zoom = 0;
}

Camera::Camera(int worldHeight, int worldWidth, int screenHeight, int screenWidth) {
	this->worldHeight = worldHeight;
	this->worldWidth = worldWidth;
	this->screenHeight = screenHeight;
	this->screenWidth = screenWidth;
	zoom = 0;
	pos = { 0, 0 };

	if (worldHeight / screenHeight > worldWidth / screenWidth) {
		orthoWidth = (worldHeight / screenHeight) * screenWidth;
		orthoHeight = (worldHeight / screenHeight) * screenHeight;
	} else {
		orthoWidth = worldWidth / screenWidth * screenWidth;
		orthoHeight = worldWidth / screenWidth * screenHeight;
	}
}

Camera::~Camera() {
	scrollEvents.Clear();
}

void Camera::Init(GLFWwindow* window) {
	glfwSetScrollCallback(window, scroll_callback);
}

glm::mat4 Camera::GetView() {
	return translate(glm::mat4(1), glm::vec3(pos, 0));
}

glm::mat4 Camera::GetProj() {
	float wZoom = float(orthoWidth) * zoom;
	float hZoom = float(orthoHeight) * zoom;
	return glm::ortho(float(orthoWidth) + wZoom , -wZoom, float(orthoHeight) + hZoom, -hZoom);
}

glm::vec2 Camera::GetWorldPos(glm::vec2 screenPos) {
	float halfW = screenWidth / 2.0;
	float halfH = screenHeight / 2.0;
	float x = screenPos.x - halfW;
	float y = screenPos.y - halfH;

	x /= halfW;
	y /= halfH;

	glm::vec4 worldPos = inverse(GetProj() * GetView()) * glm::vec4(x, y, 0.0f, 1.0f);
	return { worldPos.x, worldPos.y };
}

glm::vec2 Camera::GetViewPos(glm::vec2 screenPos) {
	float halfW = screenWidth / 2.0;
	float halfH = screenHeight / 2.0;
	float x = screenPos.x - halfW;
	float y = screenPos.y - halfH;

	x /= halfW;
	y /= halfH;

	glm::vec4 worldPos = inverse(GetProj()) * glm::vec4(x, y, 0.0f, 1.0f);
	return { worldPos.x, worldPos.y };
}

void Camera::Update(GLFWwindow* window, float deltaTime) {
	float cameraZoomSpeed = pow(zoom + 1, 2) * 30.0f * deltaTime;

	//Translation	
	auto state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	if (state == GLFW_PRESS) {
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		glm::vec2 mousePos = GetViewPos({ x, y });
		if (oldMousePos != oldMouseFlag) {
			glm::vec2 delta = mousePos - oldMousePos;
			pos += delta;
		}
		oldMousePos = mousePos;
	} else {
		oldMousePos = oldMouseFlag;
	}

	while (!scrollEvents.Empty()) {
		zoom -= scrollEvents.Pop() * cameraZoomSpeed;
	}
	
	if (zoom <= minZoom)
		zoom = minZoom;
	if (zoom > maxZoom)
		zoom = maxZoom;
}


void Camera::scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
	scrollEvents.Push(yOffset);
}

