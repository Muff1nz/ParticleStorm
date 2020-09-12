#include "GuiEngine.h"

#include <iostream>


#include "CollisionChecker.h"
#include "Utils.h"

GuiEngine::GuiEngine(EventEngine* eventEngine, MessageSystem* messageSystem) {
	this->eventEngine = eventEngine;
	this->messageSystem = messageSystem;
}

GuiEngine::~GuiEngine() = default;

void GuiEngine::Init(Camera* camera) {
	this->camera = camera;
}

void GuiEngine::Update() {
	HandleMessages();
	
	if (eventEngine->GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
		glm::vec2 mouseWorldPos = camera->GetWorldPos(eventEngine->GetMousePos());
		//std::cout << Utils::VecToString(mouseWorldPos) + "\n";
		for (auto imageButton : imageButtons) {
			if (CollisionChecker::PointInsideRect(mouseWorldPos, imageButton->rect)) {
				//std::cout << "Button: " + imageButton->texturePath + " was clicked! \n";
				imageButton->TriggerOnClick();
			}
		}
	}

}

void GuiEngine::AddEntity(const Message& message) {
	BaseEntity* entity = static_cast<BaseEntity*>(message.payload);

	if (entity->type == ET_ImageButton) {
		ImageButtonEntity* imageButtonEntity = static_cast<ImageButtonEntity*>(entity);
		imageButtonEntity->RegisterAsObserver();
		imageButtons.push_back(imageButtonEntity);
	}
}

void GuiEngine::RemoveEntity(const Message& message) {
	BaseEntity* entity = static_cast<BaseEntity*>(message.payload);
	if (entity->type == ET_ImageButton) {
		for (int i = 0; i < imageButtons.size(); ++i) {
			if (imageButtons[i]->id == entity->id) {
				imageButtons[i]->UnregisterAsObserver();
				imageButtons.erase(imageButtons.begin() + i);
				return;
			}
		}
	}
}

void GuiEngine::HandleMessages() {
	Message message = messageSystem->PS_GetMessage(SYSTEM_GuiEngine);
	while (!message.IsEmpty()) {
		switch (message.messageType) {
		case MT_Entity_Submitted:
			AddEntity(message);
			break;
		case MT_Entity_Destroyed:
			RemoveEntity(message);
		default:
			break;
		}
		message = messageSystem->PS_GetMessage(SYSTEM_GuiEngine);
	}
}
