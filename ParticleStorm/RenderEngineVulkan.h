#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <thread>

#include "Environment.h"
#include "RenderEntity.h"
#include "RenderEngineVulkanBackend.h"
#include "Window.h"
#include "MessageQueue.h"
#include "Vertex.h"
#include "VulkanAllocator.h"
#include "TextureGenerator.h"


class RenderEngineVulkan {
public:
	//Constructor / Destructor
	RenderEngineVulkan(Environment* environment, MessageQueue* messageQueue);
	~RenderEngineVulkan();

	//Init
	void Init();

	//Cleanup
	void Dispose();

	//Threading
	void Start();
	void Join();
	
	//Accessors
	GLFWwindow* GetWindow() const;
	Window* GetComplexWindow() const;
private:
	//Particle Storm Specific Variables
	Environment* environment;
	MessageQueue* messageQueue;
	bool debugMode;

	//General
	const int MAX_FRAMES_IN_FLIGHT = 2;

	Window* window;
	bool windowSizeChanged = false;
	RenderEngineVulkanBackend* vulkanBackend;
	VulkanContext* vulkanContext;
	VulkanAllocator* vulkanAllocator;
	TextureGenerator* textureGenerator;

	//Vulkan
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<bool> commandBuffersValidState;

	//Data for the stuff that we draw
	VkBuffer quadVertexBuffer;
	VkDeviceMemory quadVertexBufferMemory;
	VkBuffer quadIndexBuffer;
	VkDeviceMemory quadIndexBufferMemory;
	VkBuffer quadLineIndexBuffer;
	VkDeviceMemory quadLineIndexBufferMemory;

	std::vector<RenderEntity*> renderEntities;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	size_t currentFrame = 0;

	void UpdateCommandBuffer(int imageIndex);
	void UpdateRenderEntities();
	void RecreateCommandBuffers();
	void RecreateSwapChain();
	static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
	bool HandleSwapChain(VkResult result, bool includeCallback);
	void DrawFrame();

	//Cleanup
	bool isDisposed;

	//Threading
	std::thread renderThead;

	void HandleMessages();

	//Threading
	void RenderThreadRun();

	//Vulkan Init
	void CreateCommandBuffers();
	void CreateSyncObjects();
	void CreateVertexBuffer();
	void CreateIndexBuffer(const std::vector<uint16_t>& indices, VkBuffer& indexBuffer, VkDeviceMemory& indexBufferMemory) const;
	void CreateRenderEntities();
	
	void InitVulkan();

	const glm::vec3 color = { 0.0f, 1.0f, 0.0f };
	const std::vector<Vertex> vertices = {
		{ { -1.0f, -1.0f }, color, { 1.0f, 0.0f } },
		{ { 1.0f, -1.0f }, color, { 0.0f, 0.0f } },
		{ { 1.0f, 1.0f }, color, { 0.0f, 1.0f } },
		{ { -1.0f, 1.0f }, color, { 1.0f, 1.0f } }
	};
	
	const std::vector<uint16_t> QuadIndices = {
		0, 1, 2, 2, 3, 0
	};
	const std::vector<uint16_t> LineQuadIndices = {
		0, 1, 2, 3, 0
	};
};



