#pragma once


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <optional>
#include "Window.h"
#include "RenderDataVulkanContext.h"
#include "VulkanAllocator.h"

class Environment;

class RenderEngineVulkanBackend {
public:
	RenderEngineVulkanBackend();	
	~RenderEngineVulkanBackend();

	void Init(Window* window);
	void RecreateSwapChain();
	void Dispose();

	RenderDataVulkanContext* GetRenderDataVulkanContext();
	VulkanAllocator* GetVulkanAllocator();
	
private:
	bool isDisposed = false;
	bool isSwapChainDisposed = false;

	Window* window;
	RenderDataVulkanContext* vulkanContext;
	VulkanAllocator* vulkanAllocator;
	
	//Internal structs
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		bool IsComplete() const {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};
	
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities{};
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	//Debug
	const std::vector<const char*> validationLayers = { "VK_LAYER_LUNARG_standard_validation" };
#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif
	VkDebugUtilsMessengerEXT callback;

	//Backend variables
	const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	VkInstance instance;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkSurfaceKHR surface;
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	VkRenderPass renderPass;
	std::vector<VkFramebuffer> swapChainFrameBuffers;
	VkCommandPool commandPool;

	//Functions
	void DisposeSwapChain();	
	void CopyToVulkanContext() const;

	//Init step 1
	void CreateInstance();
	std::vector<const char*> GetRequiredExtensions() const;
	bool CheckValidationLayerSupport() const;

	//Init step 2
	void SetupDebugCallback();
	static VkBool32 DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
	static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback);
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator);

	//Init step 3
	void CreateSurface();

	//Init step 4
	void PickPhysicalDevice();
	bool IsDeviceSuitable(VkPhysicalDevice device) const;
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device) const;

	//Init step 5
	void CreateLogicalDevice();

	//Init step 6
	void CreateSwapChain();
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device) const;
	static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	static VkPresentModeKHR ChooseSwapPresentMode(std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device) const;

	//Init step 7
	void CreateImageViews();

	//Init step 8
	void CreateRenderPass();

	//Init step 9
	void CreateFrameBuffers();

	//Init step 10
	void CreateCommandPool();
};
