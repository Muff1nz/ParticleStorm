#include "Camera.h"
#include <ext/matrix_transform.inl>
#include <ext/matrix_clip_space.inl>


Camera::Camera(Window* window) {
	this->window = window;
	pos = { 0, 0 };
	ResetCamera();
}

Camera::~Camera() = default;

glm::mat4 Camera::GetView() {
	glm::vec2 offset = { GetOrthoWidth() / 2, GetOrthoHeight() / 2 };
	return translate(glm::mat4(1), glm::vec3(pos + offset, 0));
}

glm::mat4 Camera::GetProj() {
	float wZoom = float(GetOrthoWidth()) * zoom;
	float hZoom = float(GetOrthoHeight()) * zoom;
	return glm::ortho(-wZoom, float(GetOrthoWidth()) + wZoom,  float(GetOrthoHeight()) + hZoom, -hZoom);
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
	const float hRatio = float(GetWorldHeight()) / window->GetHeight();
	const float wRatio = float(GetWorldWidth()) / window->GetWidth();
	return hRatio > wRatio ? hRatio : wRatio;
}

int Camera::GetOrthoHeight() const {
	return GetPixelToWorld() * window->GetHeight();
}

int Camera::GetOrthoWidth() const {
	return GetPixelToWorld() * window->GetWidth();
}

void Camera::Update(float deltaTime) {
	if (isStatic)
		return;
	
	float cameraZoomSpeed = pow(zoom + 1, 2) * 30.0f * deltaTime;
	
	if (eventEngine->GetMouseButton(GLFW_MOUSE_BUTTON_RIGHT)) {
		auto mousePos = GetViewPos(eventEngine->GetMousePos());
		pos += mousePos - oldMousePos;
	}
	oldMousePos = GetViewPos(eventEngine->GetMousePos());
	
	zoom -= eventEngine->GetMouseScrollDelta() * cameraZoomSpeed;	
	
	if (zoom <= minZoom)
		zoom = minZoom;
	if (zoom > maxZoom)
		zoom = maxZoom;

	if (eventEngine->GetKeyDown(GLFW_KEY_R))
		ResetCamera();
}

void Camera::SetEventEngine(EventEngine* eventEngine) {
	this->eventEngine = eventEngine;
}

void Camera::SetWorld(WorldEntity* world) {
	this->world = world;
	ResetCamera();
}

void Camera::SetStatic(bool isStatic) {
	this->isStatic = isStatic;
}

void Camera::ResetCamera() {
	zoom = 0;
	pos = { -GetWorldWidth() / 2, -GetWorldHeight() / 2 };
}

int Camera::GetWorldHeight() const {
	return world != nullptr ? world->height : window->GetHeight();
}

int Camera::GetWorldWidth() const {
	return world != nullptr ? world->width : window->GetWidth();
}
