#include "EventEngine.h"
#include "Queue.h"

Queue<double> EventEngine::scrollEvents{};

EventEngine::EventEngine(): window(nullptr) {}

EventEngine::~EventEngine() = default;

void EventEngine::Init(GLFWwindow* window) {
	this->window = window;
	glfwSetScrollCallback(window, ScrollCallback);
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

glm::vec2 EventEngine::GetMousePos() const {
	return mousePos;
}

int EventEngine::GetMouseScrollDelta() const {
	return mouseScrollDelta;
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

	mouseScrollDelta = 0;
	while (!scrollEvents.Empty()) {
		mouseScrollDelta += scrollEvents.Pop();
	}

	double x, y;
	glfwGetCursorPos(window, &x, &y);
	mousePos = { x, y };
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

void EventEngine::ScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
	scrollEvents.Push(yOffset);
}
