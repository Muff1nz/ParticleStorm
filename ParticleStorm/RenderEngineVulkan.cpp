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
#include <set>
#include <limits>
#include <fstream>
#include <gtc/matrix_transform.hpp>
#include <array>

#include "Timer.h"
#include "RenderEntityFactory.h"


//     _____                _                   _                   __  _____            _                   _             
//    / ____|              | |                 | |                 / / |  __ \          | |                 | |            
//   | |     ___  _ __  ___| |_ _ __ _   _  ___| |_ ___  _ __     / /  | |  | | ___  ___| |_ _ __ _   _  ___| |_ ___  _ __ 
//   | |    / _ \| '_ \/ __| __| '__| | | |/ __| __/ _ \| '__|   / /   | |  | |/ _ \/ __| __| '__| | | |/ __| __/ _ \| '__|
//   | |___| (_) | | | \__ \ |_| |  | |_| | (__| || (_) | |     / /    | |__| |  __/\__ \ |_| |  | |_| | (__| || (_) | |   
//    \_____\___/|_| |_|___/\__|_|   \__,_|\___|\__\___/|_|    /_/     |_____/ \___||___/\__|_|   \__,_|\___|\__\___/|_|   
//                                                                                                                         
//                                                                                                                         

RenderEngineVulkan::RenderEngineVulkan(Environment* environment) {
	this->environment = environment;
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
	commandBuffers.resize(renderDataVulkanContext->swapChainFrameBuffers.size());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = renderDataVulkanContext->commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(renderDataVulkanContext->device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	for (size_t i = 0; i < commandBuffers.size(); i++) {
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderDataVulkanContext->renderPass;
		renderPassInfo.framebuffer = renderDataVulkanContext->swapChainFrameBuffers[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = renderDataVulkanContext->swapChainExtent;

		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		for (auto renderEntity : renderEntities)
			if (renderEntity->name != "QuadTree" || environment->renderQuadTree)
				renderEntity->BindToCommandPool(commandBuffers, quadVertexBuffer, quadIndexBuffer, indices, i);

		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
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
		if (vkCreateSemaphore(renderDataVulkanContext->device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(renderDataVulkanContext->device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(renderDataVulkanContext->device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

void RenderEngineVulkan::CreateVertexBuffer() {
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	vulkanBackend->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(renderDataVulkanContext->device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(renderDataVulkanContext->device, stagingBufferMemory);

	vulkanBackend->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, quadVertexBuffer, quadVertexBufferMemory);
	vulkanBackend->CopyBuffer(stagingBuffer, quadVertexBuffer, bufferSize);
	vkDestroyBuffer(renderDataVulkanContext->device, stagingBuffer, nullptr);
	vkFreeMemory(renderDataVulkanContext->device, stagingBufferMemory, nullptr);
}

void RenderEngineVulkan::CreateIndexBuffer() {
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	vulkanBackend->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(renderDataVulkanContext->device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(renderDataVulkanContext->device, stagingBufferMemory);

	vulkanBackend->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, quadIndexBuffer, quadIndexBufferMemory);

	vulkanBackend->CopyBuffer(stagingBuffer, quadIndexBuffer, bufferSize);

	vkDestroyBuffer(renderDataVulkanContext->device, stagingBuffer, nullptr);
	vkFreeMemory(renderDataVulkanContext->device, stagingBufferMemory, nullptr);
}

void RenderEngineVulkan::CreateRenderEntities() {
	RenderTransform* backgroundTransform = new RenderTransform();
	backgroundTransform->pos = new glm::vec2(environment->worldWidth / 2, environment->worldHeight / 2);
	backgroundTransform->scale = new glm::vec2(environment->worldWidth / 2, environment->worldHeight / 2);
	backgroundTransform->instanceCount = 1;	
	renderEntities.push_back(RenderEntityFactory::CreateRenderEntity(vulkanBackend, renderDataVulkanContext, backgroundTransform, "backgroundVert.spv", "backgroundFrag.spv"));

	RenderTransform* particlesTransform = new RenderTransform();
	particlesTransform->pos = environment->particlePos;
	particlesTransform->scale = new glm::vec2[environment->particleCount];
	std::fill_n(particlesTransform->scale, environment->particleCount, glm::vec2(environment->particleRadius, environment->particleRadius));
	particlesTransform->instanceCount = environment->particleCount;
	renderEntities.push_back(RenderEntityFactory::CreateRenderEntity(vulkanBackend, renderDataVulkanContext, particlesTransform, "particleVert.spv", "particleFrag.spv"));

	RenderTransform* quadTreeTransform = new RenderTransform();
	const int quadCount = 1000;
	quadTreeTransform->pos = new glm::vec2[quadCount];
	quadTreeTransform->scale = new glm::vec2[quadCount];
	quadTreeTransform->instanceCount = quadCount;
	auto quadTree = RenderEntityFactory::CreateRenderEntity(vulkanBackend, renderDataVulkanContext, quadTreeTransform, "quadVert.spv", "quadFrag.spv");
	quadTree->name = "QuadTree";
	renderEntities.push_back(quadTree);

}

void RenderEngineVulkan::InitVulkan() {
	CreateVertexBuffer();
	CreateIndexBuffer();
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
	window->InitWindow(environment->screenHeight, environment->screenWidth, environment->fullScreen);

	vulkanBackend = new RenderEngineVulkanBackend();
	vulkanBackend->Init(window);
	renderDataVulkanContext = vulkanBackend->GetRenderDataVulkanContext();

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

	auto device = renderDataVulkanContext->device;

	vkDeviceWaitIdle(device);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device, inFlightFences[i], nullptr);
	}



	vkDestroyBuffer(device, quadIndexBuffer, nullptr);
	vkFreeMemory(device, quadIndexBufferMemory, nullptr);
	vkDestroyBuffer(device, quadVertexBuffer, nullptr);
	vkFreeMemory(device, quadVertexBufferMemory, nullptr);


	for (int i = 0; i < renderEntities.size(); ++i) {
		delete renderEntities[i];
	}

	vulkanBackend->Dispose();
	window->Dispose();

	isDisposed = true;

	delete renderDataVulkanContext;
	delete window;
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
	renderPassInfo.renderPass = renderDataVulkanContext->renderPass;
	renderPassInfo.framebuffer = renderDataVulkanContext->swapChainFrameBuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = renderDataVulkanContext->swapChainExtent;

	VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	for (auto renderEntity : renderEntities)
		if (renderEntity->name != "QuadTree" || environment->renderQuadTree)
			renderEntity->BindToCommandPool(commandBuffers, quadVertexBuffer, quadIndexBuffer, indices, imageIndex);

	vkCmdEndRenderPass(commandBuffers[imageIndex]);

	if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}

void RenderEngineVulkan::CalculateQuadTreeTransform(RenderDataCore* renderDataCore) {
	for (int i = 0; i < renderDataCore->transform.instanceCount; ++i) {
		if (i < environment->quadRects.size()) {
			renderDataCore->transform.pos[i] = { environment->quadRects[i].x + environment->quadRects[i].halfW, environment->quadRects[i].y + environment->quadRects[i].halfH };
			renderDataCore->transform.scale[i] = { environment->quadRects[i].halfW, environment->quadRects[i].halfH };
		} else {
			renderDataCore->transform.pos[i] = { 0, 0 };
			renderDataCore->transform.scale[i] = { 0, 0 };
		}
	}	
}

void RenderEngineVulkan::DrawFrame() {
	auto device = renderDataVulkanContext->device;

	vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
	vkResetFences(device, 1, &inFlightFences[currentFrame]);
		
	uint32_t imageIndex;
	vkAcquireNextImageKHR(device, renderDataVulkanContext->swapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	UpdateCommandBuffer(imageIndex);

	for (auto renderEntity : renderEntities) {
		if (environment->renderQuadTree && renderEntity->name == "QuadTree")
			CalculateQuadTreeTransform(renderEntity->GetRenderDataCore());
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

	if (vkQueueSubmit(renderDataVulkanContext->graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { renderDataVulkanContext->swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional

	vkQueuePresentKHR(renderDataVulkanContext->presentQueue, &presentInfo);

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

	++environment->stats.renderUpdateTotalLastSecond;
}

void RenderEngineVulkan::RenderThreadRun() {
	Timer timer(99999.0f, 1.0f/144.0f);
	while (!environment->done) {
		timer.DeltaTime();
		DrawFrame();
	}
}
