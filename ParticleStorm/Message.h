#pragma once

#include <string>

#include "SystemComponent.h"
#include "MessageType.h"

struct Message {
	SystemComponent sender;
	SystemComponent receiver;
	MessageType messageType;
	std::string message;
	void* payload;

	Message() : Message(SYSTEM_None, SYSTEM_None, MT_Empty, "", nullptr) {}

	Message(SystemComponent sender, MessageType messageType) : Message(sender, SYSTEM_None, messageType, "", nullptr) {}

	Message(SystemComponent sender, MessageType messageType, std::string message) : Message(sender, SYSTEM_None, messageType, message, nullptr) {}

	Message(SystemComponent sender, SystemComponent receiver, MessageType messageType) : Message(sender, receiver, messageType, "", nullptr) {}

	Message(SystemComponent sender, SystemComponent receiver, MessageType messageType, std::string message) : Message(sender, receiver, messageType, message, nullptr) {}

	Message(SystemComponent sender, SystemComponent receiver, MessageType messageType, void* payload) : Message(sender, receiver, messageType, "", payload) {}

	Message(SystemComponent sender, SystemComponent receiver, MessageType messageType, std::string message, void* payload) {
		this->sender = sender;
		this->receiver = receiver;
		this->messageType = messageType;
		this->message = message;
		this->payload = payload;
	}

	bool IsEmpty() const {
		return messageType == MT_Empty;
	}
};
