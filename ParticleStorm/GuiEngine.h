#pragma once
#include <vector>

#include "ImageButtonEntity.h"
#include "EventEngine.h"
#include "GuiEngine.h"
#include "Camera.h"

class GuiEngine
{
public:
	GuiEngine(EventEngine* eventEngine, MessageSystem* messageSystem);
	~GuiEngine();

	void Init(Camera* camera);
	void Update();
private:
	EventEngine* eventEngine;
	Camera* camera;
	MessageSystem* messageSystem;
	
	std::vector<ImageButtonEntity*>  imageButtons;

	void AddEntity(const Message& message);
	void RemoveEntity(const Message& message);
	
	void HandleMessages();
};

