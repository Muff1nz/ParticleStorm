//    _____            _           _           
//   |_   _|          | |         | |          
//     | |  _ __   ___| |_   _  __| | ___  ___ 
//     | | | '_ \ / __| | | | |/ _` |/ _ \/ __|
//    _| |_| | | | (__| | |_| | (_| |  __/\__ \
//   |_____|_| |_|\___|_|\__,_|\__,_|\___||___/
//                                             
//                                             

#include "RenderEngineVulkan.h"

#include <set>
#include <limits>
#include <fstream>
#include <gtc/matrix_transform.hpp>

#include "Timer.h"
#include "RenderEntityFactory.h"
#include "ConstStrings.h"


//     _____                _                   _                   __  _____            _                   _             
//    / ____|              | |                 | |                 / / |  __ \          | |                 | |            
//   | |     ___  _ __  ___| |_ _ __ _   _  ___| |_ ___  _ __     / /  | |  | | ___  ___| |_ _ __ _   _  ___| |_ ___  _ __ 
//   | |    / _ \| '_ \/ __| __| '__| | | |/ __| __/ _ \| '__|   / /   | |  | |/ _ \/ __| __| '__| | | |/ __| __/ _ \| '__|
//   | |___| (_) | | | \__ \ |_| |  | |_| | (__| || (_) | |     / /    | |__| |  __/\__ \ |_| |  | |_| | (__| || (_) | |   
//    \_____\___/|_| |_|___/\__|_|   \__,_|\___|\__\___/|_|    /_/     |_____/ \___||___/\__|_|   \__,_|\___|\__\___/|_|   
//                                                                                                                         
//                                                                                                                         

RenderEngineVulkan::RenderEngineVulkan(Environment* environment, MessageQueue* messageQueue) {
	this->environment = environment;
	this->messageQueue = messageQueue;
}

RenderEngineVulkan::~RenderEngineVulkan() {
	Dispose();
}


//   __      __    _ _                 _____       _ _   
//   \ \    / /   | | |               |_   _|     (_) |  
//    \ \  / /   _| | | ____ _ _ __     | |  _ __  _| |_ 
//     \ \/ / | | | | |/ / _` | '_ \    | | | '_ \| | __|
//      \  /| |_| | |   < (_| | | | |  _| |_| | | | | |_ 
//       \/  \__,_|_|_|\_\__,_|_| |_| |_____|_| |_|_|\__|
//                                                       
//                                                       



void RenderEngineVulkan::CreateCommandBuffers() {
	commandBuffers.resize(vulkanContext->swapChainFrameBuffers.size());
	commandBuffersValidState.resize(vulkanContext->swapChainFrameBuffers.size());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = vulkanContext->commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(vulkanContext->device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	for (size_t i = 0; i < commandBuffers.size(); i++) {
		UpdateCommandBuffer(i);
	}
}

void RenderEngineVulkan::CreateSyncObjects() {
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(vulkanContext->device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(vulkanContext->device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(vulkanContext->device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

void RenderEngineVulkan::CreateVertexBuffer() {
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	vulkanAllocator->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(vulkanContext->device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(vulkanContext->device, stagingBufferMemory);

	vulkanAllocator->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, quadVertexBuffer, quadVertexBufferMemory);
	vulkanAllocator->CopyBuffer(stagingBuffer, quadVertexBuffer, bufferSize);
	vkDestroyBuffer(vulkanContext->device, stagingBuffer, nullptr);
	vkFreeMemory(vulkanContext->device, stagingBufferMemory, nullptr);
}

void RenderEngineVulkan::CreateIndexBuffer(const std::vector<uint16_t>& indices, VkBuffer& indexBuffer, VkDeviceMemory& indexBufferMemory) const {
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	vulkanAllocator->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(vulkanContext->device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(vulkanContext->device, stagingBufferMemory);

	vulkanAllocator->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	vulkanAllocator->CopyBuffer(stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(vulkanContext->device, stagingBuffer, nullptr);
	vkFreeMemory(vulkanContext->device, stagingBufferMemory, nullptr);
}

void RenderEngineVulkan::CreateRenderEntities() {
	RenderEntityFactory factory(vulkanContext, vulkanAllocator);

	RenderEntityCreateInfo createInfoBackground;
	createInfoBackground.vertexShader = "backgroundVert.spv";
	createInfoBackground.fragmentShader = "backgroundFrag.spv";
	createInfoBackground.texturePath = "Textures/BackGround.png";
	createInfoBackground.renderMode = Triangles;
	createInfoBackground.vertexBuffer = quadVertexBuffer;
	createInfoBackground.indexBuffer = quadIndexBuffer;
	createInfoBackground.indexCount = static_cast<uint32_t>(QuadIndices.size());

	RenderTransform* backgroundTransform = new RenderTransform();
	backgroundTransform->pos = new glm::vec2(environment->worldWidth / 2, environment->worldHeight / 2);
	backgroundTransform->scale = new glm::vec2(environment->worldWidth / 2, environment->worldHeight / 2);
	backgroundTransform->objectCount = 1;	
	renderEntities.push_back(factory.CreateRenderEntity(createInfoBackground, backgroundTransform, false));

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	RenderEntityCreateInfo createInfoParticles;
	createInfoParticles.vertexShader = "particleVert.spv";
	createInfoParticles.fragmentShader = "particleFrag.spv";
	createInfoParticles.renderMode = Triangles;
	createInfoParticles.vertexBuffer = quadVertexBuffer;
	createInfoParticles.indexBuffer = quadIndexBuffer;
	createInfoParticles.indexCount = static_cast<uint32_t>(QuadIndices.size());

	RenderTransform* particlesTransform = new RenderTransform();
	particlesTransform->pos = environment->particlePos;
	particlesTransform->scale = new glm::vec2(environment->particleRadius, environment->particleRadius);
	particlesTransform->staticScale = true;
	particlesTransform->objectCount = environment->particleCount;
	renderEntities.push_back(factory.CreateRenderEntity(createInfoParticles, particlesTransform, false));

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	RenderEntityCreateInfo createInfoQuadTree;
	createInfoQuadTree.vertexShader = "quadVert.spv";
	createInfoQuadTree.fragmentShader = "quadFrag.spv";
	createInfoQuadTree.renderMode = Lines;
	createInfoQuadTree.vertexBuffer = quadVertexBuffer;
	createInfoQuadTree.indexBuffer = quadLineIndexBuffer;
	createInfoQuadTree.indexCount = static_cast<uint32_t>(LineQuadIndices.size());

	RenderTransform* quadTreeTransform = new RenderTransform();
	quadTreeTransform->pos = environment->quadPos;
	quadTreeTransform->scale = environment->quadScale;
	quadTreeTransform->objectCount = environment->debugQuadSize;
	renderEntities.push_back(factory.CreateRenderEntity(createInfoQuadTree, quadTreeTransform, true));
}

void RenderEngineVulkan::InitVulkan() {
	CreateVertexBuffer();
	CreateIndexBuffer(QuadIndices, quadIndexBuffer, quadIndexBufferMemory);
	CreateIndexBuffer(LineQuadIndices, quadLineIndexBuffer, quadLineIndexBufferMemory);
	CreateRenderEntities();
	CreateCommandBuffers();
	CreateSyncObjects();
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

	window = new Window();
	window->InitWindow(1200, 2800, environment->fullScreen);
	glfwSetWindowUserPointer(window->GetWindow(), this);
	glfwSetFramebufferSizeCallback(window->GetWindow(), FramebufferResizeCallback);
	environment->camera = Camera(environment->worldHeight, environment->worldWidth, window);


	vulkanBackend = new RenderEngineVulkanBackend();
	vulkanBackend->Init(window);
	vulkanContext = vulkanBackend->GetVulkanContext();
	vulkanAllocator = new VulkanAllocator(vulkanContext);

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
	if (isDisposed)
		return;

	auto device = vulkanContext->device;

	vkDeviceWaitIdle(device);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device, inFlightFences[i], nullptr);
	}

	vkDestroyBuffer(device, quadLineIndexBuffer, nullptr);
	vkFreeMemory(device, quadLineIndexBufferMemory, nullptr);
	vkDestroyBuffer(device, quadIndexBuffer, nullptr);
	vkFreeMemory(device, quadIndexBufferMemory, nullptr);
	vkDestroyBuffer(device, quadVertexBuffer, nullptr);
	vkFreeMemory(device, quadVertexBufferMemory, nullptr);


	for (int i = 0; i < renderEntities.size(); ++i) {
		delete renderEntities[i];
	}

	vulkanBackend->Dispose();
	window->Dispose();
	
	delete window;
	delete vulkanAllocator;
	delete vulkanBackend;
	
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

GLFWwindow* RenderEngineVulkan::GetWindow() const {
	return window->GetWindow();
}


//    _______ _                        _ _             
//   |__   __| |                      | (_)            
//      | |  | |__  _ __ ___  __ _  __| |_ _ __   __ _ 
//      | |  | '_ \| '__/ _ \/ _` |/ _` | | '_ \ / _` |
//      | |  | | | | | |  __/ (_| | (_| | | | | | (_| |
//      |_|  |_| |_|_|  \___|\__,_|\__,_|_|_| |_|\__, |
//                                                __/ |
//                                               |___/ 

void RenderEngineVulkan::Start() {
	renderThead = std::thread([=] {RenderThreadRun(); });
}

void RenderEngineVulkan::Join() {
	renderThead.join();
}

void RenderEngineVulkan::UpdateCommandBuffer(int imageIndex) {
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = vulkanContext->renderPass;
	renderPassInfo.framebuffer = vulkanContext->swapChainFrameBuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = vulkanContext->swapChainExtent;

	VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	for (auto renderEntity : renderEntities)
		if (!renderEntity->IsDebugEntity() || debugMode)
			renderEntity->BindToCommandPool(commandBuffers, imageIndex);

	vkCmdEndRenderPass(commandBuffers[imageIndex]);

	if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}

	commandBuffersValidState[imageIndex] = true;
}

void RenderEngineVulkan::UpdateRenderEntities() {
	RenderEntityFactory factory(vulkanContext, vulkanAllocator);
	for (auto renderEntity : renderEntities) 
		factory.RecreateGraphicsPipeline(renderEntity);
}

void RenderEngineVulkan::RecreateCommandBuffers() {
	vkFreeCommandBuffers(vulkanContext->device, vulkanContext->commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
	CreateCommandBuffers();
}

void RenderEngineVulkan::RecreateSwapChain() {
	
	while (window->IsMinimized()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	vkDeviceWaitIdle(vulkanContext->device);
	vulkanBackend->RecreateSwapChain();
	UpdateRenderEntities();
	RecreateCommandBuffers();

	windowSizeChanged = false;
}

void RenderEngineVulkan::FramebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto engine = reinterpret_cast<RenderEngineVulkan*>(glfwGetWindowUserPointer(window));
	engine->windowSizeChanged = true;
}

bool RenderEngineVulkan::HandleSwapChain(VkResult result, bool includeCallback) {
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || windowSizeChanged && includeCallback) {
		RecreateSwapChain(); 
		return true;
	}
	if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}
	return false;
}

void RenderEngineVulkan::DrawFrame() {
	auto device = vulkanContext->device;

	vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
		
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device, vulkanContext->swapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (HandleSwapChain(result, false)) return;

	if (!commandBuffersValidState[imageIndex])
		UpdateCommandBuffer(imageIndex);

	for (auto renderEntity : renderEntities) {
		if (!renderEntity->IsDebugEntity() || debugMode)
			renderEntity->UpdateBuffers(imageIndex, &environment->camera);
	}

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(device, 1, &inFlightFences[currentFrame]);

	if (vkQueueSubmit(vulkanContext->graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { vulkanContext->swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional

	result = vkQueuePresentKHR(vulkanContext->presentQueue, &presentInfo);

	if (HandleSwapChain(result, true)) return;

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

	++environment->stats.renderUpdateTotalLastSecond;
}

void RenderEngineVulkan::HandleMessages() {
	Message message = messageQueue->PS_GetMessage(SYSTEM_RenderEngine);
	while (!message.IsEmpty()) {
		switch (message.messageType) {
		case MT_DebugModeToggle:
			debugMode = !debugMode;
			for (int i = 0; i < commandBuffersValidState.size(); ++i) {
				commandBuffersValidState[i] = false;
			}
			break;
		case MT_FullScreenToggle:
			window->ToggleFullscreen();
			break;
		default:;
		}
		message = messageQueue->PS_GetMessage(SYSTEM_RenderEngine);
	}
}

void RenderEngineVulkan::RenderThreadRun() {
	Timer timer(99999.0f, 1.0f/144.0f);
	while (!environment->done) {
		timer.DeltaTime();
		HandleMessages();
		window->UpdateMetaData();
		DrawFrame();
	}
}
