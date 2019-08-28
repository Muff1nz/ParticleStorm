#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <optional>
#include <thread>
#include <array>

#include "Environment.h"
#include "RenderEntity.h"

//TODO: https://vulkan-tutorial.com/Drawing_a_triangle/Swap_chain_recreation
class RenderEngineVulkan {
public:
	//Constructor / Destructor
	RenderEngineVulkan(Environment* environment);
	~RenderEngineVulkan();

	//Init
	void Init();

	//Cleanup
	void Dispose();

	//Threading
	void Start();
	void Join();
	void DrawFrame();

	//Accessors
	GLFWwindow* GetWindow() const;
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
private:
	//Particle Storm Specific Variables
	Environment* environment;

	//General
	const int MAX_FRAMES_IN_FLIGHT = 2;

	//Vulkan Debug
	const std::vector<const char*> validationLayers = { "VK_LAYER_LUNARG_standard_validation" };
#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif
	VkDebugUtilsMessengerEXT callback;

	//Vulkan
	//What could be moved into "VulkanBackend"
	const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME	};
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
	std::vector<VkCommandBuffer> commandBuffers;

	//Data for the stuff that we draw
	VkBuffer quadVertexBuffer;
	VkDeviceMemory quadVertexBufferMemory;
	VkBuffer quadIndexBuffer;
	VkDeviceMemory quadIndexBufferMemory;

	std::vector<RenderEntity*> renderEntities;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	size_t currentFrame = 0;

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
		std::optional<uint32_t> presentFamily;
		bool IsComplete() const {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};
	std::vector<const char*> GetRequiredExtensions() const;
	void CreateInstance();
	void PickPhysicalDevice();
	bool IsDeviceSuitable(VkPhysicalDevice device);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device) const;
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device) const;
	void CreateLogicalDevice();
	void CreateSurface();
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;
	VkPresentModeKHR ChooseSwapPresentMode(std::vector<VkPresentModeKHR> availablePresentModes) const;
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	void CreateSwapChain();
	void CreateImageViews();
	static std::vector<char> ReadFile(const std::string& filename);
	VkShaderModule CreateShaderModule(const std::vector<char>& code);
	void CreateRenderPass();
	void CreateFrameBuffers();
	void CreateCommandPool();
	void CreateCommandBuffers();
	void CreateSyncObjects();
	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	std::vector<VkVertexInputBindingDescription>CreateVertexBindingDescription(bool instancing);
	std::vector<VkVertexInputAttributeDescription> CreateVertexAttributeDescription(bool instancing);
	void CreateVertexBuffer();
	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void CreateIndexBuffer();
	void CreateRenderEntities();
	void InitVulkan();

	//Vertex data
	struct Vertex {
		glm::vec2 pos;
		glm::vec3 color;

		static VkVertexInputBindingDescription getBindingDescription() {
			VkVertexInputBindingDescription bindingDescription;
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);
			return attributeDescriptions;
		}
	};

	struct InstanceBufferObject {
		glm::mat4 MVP;

		static VkVertexInputBindingDescription getBindingDescription() {
			VkVertexInputBindingDescription bindingDescription;
			bindingDescription.binding = 1;
			bindingDescription.stride = sizeof(InstanceBufferObject);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions = {};
			for (int i = 0; i < 4; ++i) {
				attributeDescriptions[i].binding = 1;
				attributeDescriptions[i].location = i + 2;
				attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
				attributeDescriptions[i].offset = sizeof(glm::vec4) * i;
			}
			return attributeDescriptions;
		}
	};

	struct UniformBufferObject {
		glm::mat4 MVP;
	};

	const glm::vec3 color = { 0.0f, 1.0f, 0.0f };
	const std::vector<Vertex> vertices = {
		{ { -1.0f, -1.0f }, color },
		{ { 1.0f, -1.0f }, color },
		{ { 1.0f, 1.0f }, color },
		{ { -1.0f, 1.0f }, color }
	};
	
	const std::vector<uint16_t> indices = {
		0, 1, 2, 2, 3, 0
	};

	InstanceBufferObject* MVP_Array;

	//Init GLFW
	void InitWindow();

	//Threading
	void RenderThreadRun();
};



