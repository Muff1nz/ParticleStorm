#include "SingleTimeCommand.h"



SingleTimeCommand::SingleTimeCommand(VulkanContext* vulkanContext) {
	this->vulkanContext = vulkanContext;
}

SingleTimeCommand::~SingleTimeCommand() = default;

VkCommandBuffer SingleTimeCommand::GetCommandBuffer() const {
	return commandBuffer;
}

void SingleTimeCommand::Begin() {
	VkCommandBufferAllocateInfo allocInfo = {}; //TODO: Based on code in copy buffer. Can simplify that function
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = vulkanContext->commandPool;
	allocInfo.commandBufferCount = 1;

	vkAllocateCommandBuffers(vulkanContext->device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);
}

void SingleTimeCommand::End() const {
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(vulkanContext->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(vulkanContext->graphicsQueue);

	vkFreeCommandBuffers(vulkanContext->device, vulkanContext->commandPool, 1, &commandBuffer);
}