#include "EventEngine.h"


EventEngine::EventEngine() {
}

EventEngine::~EventEngine() = default;

void EventEngine::Init(GLFWwindow* window) {
	this->window = window;
}

bool EventEngine::GetKeyDown(int key) {
	HookKeyIfNeeded(key);
	return keyStates[key] && !keyStatesGhost[key];
}

bool EventEngine::GetKey(int key) {
	HookKeyIfNeeded(key);
	return keyStates[key];
}

bool EventEngine::GetKeyUp(int key) {
	HookKeyIfNeeded(key);
	return !keyStates[key] && keyStatesGhost[key];
}

bool EventEngine::GetMouseButtonDown(int mouseButton) {
	HookMouseButtonIfNeeded(mouseButton);
	return mouseButtonStates[mouseButton] && !mouseButtonStatesGhost[mouseButton];
}

bool EventEngine::GetMouseButton(int mouseButton) {
	HookMouseButtonIfNeeded(mouseButton);
	return mouseButtonStates[mouseButton];
}

bool EventEngine::GetMouseButtonUp(int mouseButton) {
	HookMouseButtonIfNeeded(mouseButton);
	return !mouseButtonStates[mouseButton] && mouseButtonStatesGhost[mouseButton];
}

void EventEngine::Update() {
	for (auto key : keyHooks) {
		keyStatesGhost[key] = keyStates[key];
		keyStates[key] = glfwGetKey(window, key);
	}

	for (auto mouseButton : mouseButtonHooks) {
		mouseButtonStatesGhost[mouseButton] = mouseButtonStates[mouseButton];
		mouseButtonStates[mouseButton] = glfwGetMouseButton(window, mouseButton);
	}
}

void EventEngine::HookKeyIfNeeded(int key) {
	if (std::find(keyHooks.begin(), keyHooks.end(), key) != keyHooks.end())
		return;

	keyHooks.emplace_back(key);

	auto state = glfwGetKey(window, key);
	keyStatesGhost.insert({ key, state });
	keyStates.insert({ key, state });
}

void EventEngine::HookMouseButtonIfNeeded(int mouseButton) {
	if (std::find(mouseButtonHooks.begin(), mouseButtonHooks.end(), mouseButton) != mouseButtonHooks.end())
		return;

	mouseButtonHooks.emplace_back(mouseButton);

	auto state = glfwGetMouseButton(window, mouseButton);
	mouseButtonStatesGhost.insert({ mouseButton, state });
	mouseButtonStates.insert({ mouseButton, state });
}
