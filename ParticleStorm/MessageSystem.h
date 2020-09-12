#pragma once

#include <vector>

#include "Message.h"
#include "Queue.h"
#include <mutex>

class MessageSystem {
public:
	MessageSystem();
	~MessageSystem();

	void PS_SendMessage(Message message);
	void PS_BroadcastMessage(Message message);
	Message PS_GetMessage(SystemComponent receiver);
private:
	static const int supportedComponentsCount = 7;
	const SystemComponent supportedComponents[supportedComponentsCount] = { SYSTEM_SessionManager, SYSTEM_PhysicsEngine, SYSTEM_RenderEngine, SYSTEM_EventEngine, SYSTEM_EntityEngine, SYSTEM_CoreEngine, SYSTEM_GuiEngine };

	std::mutex queueLock{};
	std::vector<std::pair<SystemComponent, Queue<Message>>> queues;
};

