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
