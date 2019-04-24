#include "Camera.h"
#include <ext/matrix_transform.inl>
#include <ext/matrix_clip_space.inl>

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
}

void Camera::Init(GLFWwindow* window) {
	//glfwSetScrollCallback(window, scroll_callback);
}

glm::mat4 Camera::GetView() {
	return translate(glm::mat4(1), glm::vec3(pos, 0));
}

glm::mat4 Camera::GetProj() {
	float wZoom = float(screenWidth) * zoom;
	float hZoom = float(screenHeight) * zoom;
	return glm::ortho(float(screenWidth) + wZoom , -wZoom, float(screenHeight) + hZoom, -hZoom);
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
	cameraZoomSpeed = zoom <= 1 ? 0.5f : 5.0f;

	ProcessKeyboardInput(window, deltaTime);
	ProcessMouseInput(window, deltaTime);
	
	if (zoom <= -0.49f)
		zoom = -0.49f;
}

void Camera::ProcessKeyboardInput(GLFWwindow* window, float deltaTime) {
	//Translation
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		pos.x -= cameraSpeed * deltaTime;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		pos.x += cameraSpeed * deltaTime;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		pos.y -= cameraSpeed * deltaTime;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		pos.y += cameraSpeed * deltaTime;
	}

	//ZOOM
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		zoom -= cameraZoomSpeed * deltaTime;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		zoom += cameraZoomSpeed * deltaTime;
	}
}

void Camera::ProcessMouseInput(GLFWwindow* window, float deltaTime) {
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
}

void Camera::scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
	//zoom += yoffset * cameraZoomSpeed;
}

