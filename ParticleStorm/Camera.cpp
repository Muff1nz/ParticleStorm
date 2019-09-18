#include "Camera.h"
#include <ext/matrix_transform.inl>
#include <ext/matrix_clip_space.inl>
#include "Window.h"

Queue<double> Camera::scrollEvents{};

Camera::Camera() {
	zoom = 0;
}

Camera::Camera(int worldHeight, int worldWidth, Window* window) {
	this->worldHeight = worldHeight;
	this->worldWidth = worldWidth;
	this->window = window;
	zoom = 0;
	pos = { 0, 0 };
	ResetCamera();
}

Camera::~Camera() {
	scrollEvents.Clear();
}

void Camera::Init(GLFWwindow* window) {
	glfwSetScrollCallback(window, ScrollCallback);
}

glm::mat4 Camera::GetView() {
	glm::vec2 offset = { GetOrthoWidth() / 2, GetOrthoHeight() / 2 };
	return translate(glm::mat4(1), glm::vec3(pos + offset, 0));
}

glm::mat4 Camera::GetProj() {
	float wZoom = float(GetOrthoWidth()) * zoom;
	float hZoom = float(GetOrthoHeight()) * zoom;
	return glm::ortho(float(GetOrthoWidth()) + wZoom , -wZoom, float(GetOrthoHeight()) + hZoom, -hZoom);
}

glm::vec2 Camera::GetWorldPos(glm::vec2 screenPos) {
	float halfW = window->GetWidth() / 2.0;
	float halfH = window->GetHeight() / 2.0;
	float x = screenPos.x - halfW;
	float y = screenPos.y - halfH;

	x /= halfW;
	y /= halfH;

	glm::vec4 worldPos = inverse(GetProj() * GetView()) * glm::vec4(x, y, 0.0f, 1.0f);
	return { worldPos.x, worldPos.y };
}

glm::vec2 Camera::GetViewPos(glm::vec2 screenPos) {
	float halfW = window->GetWidth() / 2.0;
	float halfH = window->GetHeight() / 2.0;
	float x = screenPos.x - halfW;
	float y = screenPos.y - halfH;

	x /= halfW;
	y /= halfH;

	glm::vec4 worldPos = inverse(GetProj()) * glm::vec4(x, y, 0.0f, 1.0f);
	return { worldPos.x, worldPos.y };
}

float Camera::GetPixelToWorld() const {
	const float hRatio = float(worldHeight) / window->GetHeight();
	const float wRatio = float(worldWidth) / window->GetWidth();
	return hRatio > wRatio ? hRatio : wRatio;
}

int Camera::GetOrthoHeight() const {
	return GetPixelToWorld() * window->GetHeight();
}

int Camera::GetOrthoWidth() const {
	return GetPixelToWorld() * window->GetWidth();
}

void Camera::Update(GLFWwindow* window, float deltaTime) {
	float cameraZoomSpeed = pow(zoom + 1, 2) * 30.0f * deltaTime;

	//Translation	
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
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

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		ResetCamera();
}


void Camera::ResetCamera() {
	zoom = 0;
	pos = { -worldWidth / 2, -worldHeight / 2 }; 
}

void Camera::ScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
	scrollEvents.Push(yOffset);
}

