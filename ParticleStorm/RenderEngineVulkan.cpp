//    _____            _           _           
//   |_   _|          | |         | |          
//     | |  _ __   ___| |_   _  __| | ___  ___ 
//     | | | '_ \ / __| | | | |/ _` |/ _ \/ __|
//    _| |_| | | | (__| | |_| | (_| |  __/\__ \
//   |_____|_| |_|\___|_|\__,_|\__,_|\___||___/
//                                             
//                                             

#include "RenderEngineVulkan.h"
#include <GLFW/glfw3.h>
#include "VulkanDebugCallbacks.h"

//     _____                _                   _                   __  _____            _                   _             
//    / ____|              | |                 | |                 / / |  __ \          | |                 | |            
//   | |     ___  _ __  ___| |_ _ __ _   _  ___| |_ ___  _ __     / /  | |  | | ___  ___| |_ _ __ _   _  ___| |_ ___  _ __ 
//   | |    / _ \| '_ \/ __| __| '__| | | |/ __| __/ _ \| '__|   / /   | |  | |/ _ \/ __| __| '__| | | |/ __| __/ _ \| '__|
//   | |___| (_) | | | \__ \ |_| |  | |_| | (__| || (_) | |     / /    | |__| |  __/\__ \ |_| |  | |_| | (__| || (_) | |   
//    \_____\___/|_| |_|___/\__|_|   \__,_|\___|\__\___/|_|    /_/     |_____/ \___||___/\__|_|   \__,_|\___|\__\___/|_|   
//                                                                                                                         
//                                                                                                                         

RenderEngineVulkan::RenderEngineVulkan(Environment* environment, Stats* stats) {
	this->environment = environment;
	this->stats = stats;
	particlesRenderCopy = new glm::vec2[environment->circleCount];
}

RenderEngineVulkan::~RenderEngineVulkan() {
	Dispose();
}

//   __      __    _ _                 _____       _                 
//   \ \    / /   | | |               |  __ \     | |                
//    \ \  / /   _| | | ____ _ _ __   | |  | | ___| |__  _   _  __ _ 
//     \ \/ / | | | | |/ / _` | '_ \  | |  | |/ _ \ '_ \| | | |/ _` |
//      \  /| |_| | |   < (_| | | | | | |__| |  __/ |_) | |_| | (_| |
//       \/  \__,_|_|_|\_\__,_|_| |_| |_____/ \___|_.__/ \__,_|\__, |
//                                                              __/ |
//                                                             |___/ 

VKAPI_ATTR VkBool32 VKAPI_CALL RenderEngineVulkan::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}

bool RenderEngineVulkan::CheckValidationLayerSupport() const {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

void RenderEngineVulkan::SetupDebugCallback() {
	if (!enableValidationLayers) return;
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
	createInfo.pUserData = nullptr; // Optional

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &callback) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug callback!");
	}
}

VkResult RenderEngineVulkan::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pCallback);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void RenderEngineVulkan::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, callback, pAllocator);
	}
}

//   __      __    _ _                 _____       _ _   
//   \ \    / /   | | |               |_   _|     (_) |  
//    \ \  / /   _| | | ____ _ _ __     | |  _ __  _| |_ 
//     \ \/ / | | | | |/ / _` | '_ \    | | | '_ \| | __|
//      \  /| |_| | |   < (_| | | | |  _| |_| | | | | |_ 
//       \/  \__,_|_|_|\_\__,_|_| |_| |_____|_| |_|_|\__|
//                                                       
//                                                       

std::vector<const char*> RenderEngineVulkan::GetRequiredExtensions() const {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

void RenderEngineVulkan::CreateInstance() {
	if (enableValidationLayers && !CheckValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}

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

	auto extensions = GetRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	} else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}
}

void RenderEngineVulkan::PickPhysicalDevice() {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (const auto& device : devices) {
		if (IsDeviceSuitable(device)) {
			physicalDevice = device;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

bool RenderEngineVulkan::IsDeviceSuitable(VkPhysicalDevice device) {
	QueueFamilyIndices indices = FindQueueFamilies(device);
	return indices.isComplete();
}

RenderEngineVulkan::QueueFamilyIndices RenderEngineVulkan::FindQueueFamilies(VkPhysicalDevice device) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}

void RenderEngineVulkan::CreateLogicalDevice() {
	QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);

	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
	queueCreateInfo.queueCount = 1;

	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceFeatures deviceFeatures = {};

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = 0;
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	} else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}
	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
}

void RenderEngineVulkan::InitVulkan() {
	CreateInstance();
	SetupDebugCallback();
	PickPhysicalDevice();
	CreateLogicalDevice();
}

//     _____ _      ________          __  _____       _ _   
//    / ____| |    |  ____\ \        / / |_   _|     (_) |  
//   | |  __| |    | |__   \ \  /\  / /    | |  _ __  _| |_ 
//   | | |_ | |    |  __|   \ \/  \/ /     | | | '_ \| | __|
//   | |__| | |____| |       \  /\  /     _| |_| | | | | |_ 
//    \_____|______|_|        \/  \/     |_____|_| |_|_|\__|
//                                                          
//                                                          

void RenderEngineVulkan::InitWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(environment->worldWidth, environment->worldHeight, "Particle Storm", nullptr, nullptr);
}

//    _____       _ _   
//   |_   _|     (_) |  
//     | |  _ __  _| |_ 
//     | | | '_ \| | __|
//    _| |_| | | | | |_ 
//   |_____|_| |_|_|\__|
//                      
//                      

void RenderEngineVulkan::Init() {
	isDisposed = false;
	InitWindow();
	InitVulkan();
}

//     _____ _                              
//    / ____| |                             
//   | |    | | ___  __ _ _ __  _   _ _ __  
//   | |    | |/ _ \/ _` | '_ \| | | | '_ \ 
//   | |____| |  __/ (_| | | | | |_| | |_) |
//    \_____|_|\___|\__,_|_| |_|\__,_| .__/ 
//                                   | |    
//                                   |_|    

void RenderEngineVulkan::Dispose() {
	if (isDisposed) return;

	vkDestroyDevice(device, nullptr);
	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, callback, nullptr);
	}
	vkDestroyInstance(instance, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();
	isDisposed = true;
}

//                                                 
//       /\                                        
//      /  \   ___ ___ ___  ___ ___  ___  _ __ ___ 
//     / /\ \ / __/ __/ _ \/ __/ __|/ _ \| '__/ __|
//    / ____ \ (_| (_|  __/\__ \__ \ (_) | |  \__ \
//   /_/    \_\___\___\___||___/___/\___/|_|  |___/
//                                                 
//                                                 

GLFWwindow* RenderEngineVulkan::GetWindow() {
	return window;
}

//    _______ _                        _ _             
//   |__   __| |                      | (_)            
//      | |  | |__  _ __ ___  __ _  __| |_ _ __   __ _ 
//      | |  | '_ \| '__/ _ \/ _` |/ _` | | '_ \ / _` |
//      | |  | | | | | |  __/ (_| | (_| | | | | | (_| |
//      |_|  |_| |_|_|  \___|\__,_|\__,_|_|_| |_|\__, |
//                                                __/ |
//                                               |___/ 

void RenderEngineVulkan::Start(bool* done) {
	//renderThead = std::thread([=] {RenderThreadRun(done); });
}

void RenderEngineVulkan::Join() {
	renderThead.join();
}

void RenderEngineVulkan::RenderThreadRun(bool* done) const {

}
