#pragma once
#include <vec3.hpp>
#include <vec2.hpp>
#include <thread>
#include "Stats.h"
#include "Environment.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <optional>

class QuadTree;

class RenderEngineVulkan {
public:
	//Constructor / Destructor
	RenderEngineVulkan(Environment* environment, Stats* stats);
	~RenderEngineVulkan();

	//Init
	void Init();

	//Cleanup
	void Dispose();

	//Threading
	void Start(bool* done);
	void Join();

	//Accessors
	GLFWwindow* GetWindow();
private:
	//Particle Storm Specific Variables
	Environment* environment;
	Stats* stats;
	glm::vec2* particlesRenderCopy;

	//Vulkan Debug
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_LUNARG_standard_validation"
	};

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif
	VkDebugUtilsMessengerEXT callback;

	//Vulkan
	VkInstance instance;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;
	VkQueue graphicsQueue;

	//GLFW
	GLFWwindow* window{};

	//Cleanup
	bool isDisposed;

	//Threading
	std::thread renderThead;

	//Vulkan Debug
	static VkBool32 DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
	bool CheckValidationLayerSupport() const;
	void SetupDebugCallback();
	static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback);
	static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator);

	//Vulkan Init
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;

		bool isComplete() {
			return graphicsFamily.has_value();
		}
	};
	std::vector<const char*> GetRequiredExtensions() const;
	void CreateInstance();
	void PickPhysicalDevice();
	static bool IsDeviceSuitable(VkPhysicalDevice device);
	static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
	void CreateLogicalDevice();
	void InitVulkan();

	//Init GLFW
	void InitWindow();

	//Threading
	void RenderThreadRun(bool* done) const;
};

