#pragma once

#include "VulkanContext.h"

class SingleTimeCommand {
public:
	SingleTimeCommand(VulkanContext* vulkanContext);
	~SingleTimeCommand();


	VkCommandBuffer GetCommandBuffer() const;
	void Begin();
	void End() const;
private:
	VkCommandBuffer commandBuffer;
	VulkanContext* vulkanContext;
};

