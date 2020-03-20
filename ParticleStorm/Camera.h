#pragma once
#include <vec2.hpp>
#include <mat4x4.hpp>
#include "Window.h"
#include "EventEngine.h"

class EventEngine;

class Camera {
public:
	Camera();
	Camera(EventEngine* eventEngine, Window* window, int worldHeight, int worldWidth);
	~Camera();

	glm::mat4 GetView();
	glm::mat4 GetProj();

	glm::vec2 GetWorldPos(glm::vec2 screenPos);
	glm::vec2 GetViewPos(glm::vec2 screenPos);

	void Update(float deltaTime);

private:	
	EventEngine* eventEngine;
	int worldHeight;
	int worldWidth;
	Window* window;

	glm::vec2 pos;

	float zoom;
	float minZoom = -0.49f;
	float maxZoom = 1.25f;
	
	glm::vec2 oldMousePos;

	float GetPixelToWorld() const;
	int GetOrthoHeight() const;
	int GetOrthoWidth() const;
	void ResetCamera();
};
