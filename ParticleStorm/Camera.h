#pragma once
#include <vec2.hpp>
#include <mat4x4.hpp>

class Camera {
public:
	Camera();
	Camera(int worldHeight, int worldWidth, int screenHeight, int screenWidth);

	glm::mat4 GetView();
	glm::mat4 GetProj();

	glm::vec2 GetWorldPos(glm::vec2 screenPos);

	glm::vec2 pos;
	float zoom;
private:
	int worldHeight;
	int worldWidth;
	int screenHeight;
	int screenWidth;
};
