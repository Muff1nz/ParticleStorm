#pragma once
#include "ImageButtonEntity.h"
#include "SessionManager.h"

class MainMenuSession : public SessionManager
{
public:
	MainMenuSession(MessageSystem* messageQueue, EventEngine* eventEngine, Camera* camera, Stats* stats);
	~MainMenuSession();

	void Init() override;
	void Update() override;
	SessionResult Complete() override;

private:
	WorldEntity* world = nullptr;
	std::vector<ImageButtonEntity*>  imageButtonEntities;
	SessionResult result;

	void OnPlayButtonClick();
	void OnPhysBenchButtonClick();
	void OnGraphBenchButtonClick();
	void OnExitButtonClick();

	void HandleEntityDestroyed(BaseEntity* entity);
	void HandleMessages() override;
};

