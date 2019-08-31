#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <thread>
#include <array>

#include "Environment.h"
#include "RenderEntity.h"
#include "RenderEngineVulkanBackend.h"
#include "Window.h"

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
private:
	//Particle Storm Specific Variables
	Environment* environment;

	//General
	const int MAX_FRAMES_IN_FLIGHT = 2;

	Window*  window;
	RenderEngineVulkanBackend* vulkanBackend;
	RenderDataVulkanContext* renderDataVulkanContext;

	//Vulkan
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


	//Cleanup
	bool isDisposed;

	//Threading
	std::thread renderThead;

	//Threading
	void RenderThreadRun();

	//Vulkan Init
	void CreateCommandBuffers();
	void CreateSyncObjects();
	void CreateVertexBuffer();
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
};



