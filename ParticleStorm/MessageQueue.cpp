#include "MessageQueue.h"
#include "EnumUtils.h"


MessageQueue::MessageQueue() {
	for (auto supportedComponent : supportedComponents) {
		queues.emplace_back(supportedComponent, Queue<Message>());
	}
}

MessageQueue::~MessageQueue() = default;

void MessageQueue::PS_SendMessage(Message message) {
	std::unique_lock<std::mutex> lock(queueLock);

	for (auto& queue : queues) {
		if (EnumUtils::HasFlag(message.receiver, queue.first)) {
			queue.second.Push(message);
		}
	}
}

void MessageQueue::PS_BroadcastMessage(Message message) {
	std::unique_lock<std::mutex> lock(queueLock);

	message.receiver = SYSTEM_None;
	for (auto supportedComponent : supportedComponents) {
		EnumUtils::AddFlag(message.receiver, supportedComponent);
	}

	for (auto& queue : queues) {
		queue.second.Push(message);
	}
}

Message MessageQueue::PS_GetMessage(SystemComponent receiver) {
	std::unique_lock<std::mutex> lock(queueLock);

	for (auto& queue : queues) {
		if (queue.first == receiver && !queue.second.Empty())
			return queue.second.Pop();
	}

	return Message();
}
