#include "RenderEngineVulkan.h"
#include <GLFW/glfw3.h>

RenderEngineVulkan::RenderEngineVulkan(Environment* environment, Stats* stats) {
	this->environment = environment;
	this->stats = stats;
	particlesRenderCopy = new glm::vec2[environment->circleCount];
}

RenderEngineVulkan::~RenderEngineVulkan() {
}

void RenderEngineVulkan::Init() {
	InitWindow();
	InitVulkan();
}

void RenderEngineVulkan::InitWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(environment->worldWidth, environment->worldHeight, "Particle Storm", nullptr, nullptr);
}

void RenderEngineVulkan::CreateInstance() {
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Particle Storm";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;


	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;
	createInfo.enabledLayerCount = 0;

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}
}

void RenderEngineVulkan::InitVulkan() {
	CreateInstance();
}

void RenderEngineVulkan::Dispose() const {
	vkDestroyInstance(instance, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();
}

GLFWwindow* RenderEngineVulkan::GetWindow() {
	return window;
}

void RenderEngineVulkan::Start(bool* done) {
	renderThead = std::thread([=] {RenderThreadRun(done); });
}

void RenderEngineVulkan::Join() {
	renderThead.join();
}

void RenderEngineVulkan::RenderThreadRun(bool* done) const {

}
