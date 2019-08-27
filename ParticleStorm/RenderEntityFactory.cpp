#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <fstream>

#include "RenderEntityFactory.h"
#include "Vertex.h"
#include "InstanceBufferObject.h"

RenderEntityFactory::RenderEntityFactory() = default;

RenderEntityFactory::~RenderEntityFactory() = default;

RenderEntity* RenderEntityFactory::CreateRenderEntity(RenderDataVulkanContext* renderDataVulkanContext, RenderTransform* transform, bool isStatic, std::string vertexShader, std::string fragmentShader) {
	RenderDataCore renderDataCore{};
	renderDataCore.transform = *transform;

	if (transform->posCount > 1) {
		RenderDataInstanced renderDataInstanced;
		CreateGraphicsPipeline(*renderDataVulkanContext, nullptr, vertexShader, fragmentShader, renderDataCore.pipeline, renderDataCore.pipelineLayout, true);		
		CreateInstanceBuffer(*renderDataVulkanContext, &renderDataInstanced);
		return new RenderEntity(renderDataVulkanContext, &renderDataCore, nullptr, &renderDataInstanced, isStatic);
	}

	RenderDataSingular renderDataSingular;
	CreateDescriptorSetLayout(*renderDataVulkanContext, &renderDataSingular);
	CreateGraphicsPipeline(*renderDataVulkanContext, &renderDataSingular, vertexShader, fragmentShader, renderDataCore.pipeline, renderDataCore.pipelineLayout, false);
	CreateUniformBuffers(*renderDataVulkanContext, &renderDataSingular);
	CreateDescriptorPool(*renderDataVulkanContext, &renderDataSingular);
	CreateDescriptorSets(*renderDataVulkanContext, &renderDataSingular);
	
	return new RenderEntity(renderDataVulkanContext, &renderDataCore, &renderDataSingular, nullptr, isStatic);
}

std::vector<char> RenderEntityFactory::ReadFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);


	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();
	return buffer;
}

VkShaderModule RenderEntityFactory::CreateShaderModule(const std::vector<char>& code, VkDevice& device) {
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

std::vector<VkVertexInputBindingDescription> RenderEntityFactory::CreateVertexBindingDescription(bool instancing) {
	std::vector<VkVertexInputBindingDescription> bindingDescriptions = {};
	bindingDescriptions.push_back(Vertex::getBindingDescription());
	if (instancing)
		bindingDescriptions.push_back(InstanceBufferObject::getBindingDescription());
	return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> RenderEntityFactory::CreateVertexAttributeDescription(bool instancing) {
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {};
	for (auto attributeDescription : Vertex::getAttributeDescriptions()) {
		attributeDescriptions.push_back(attributeDescription);
	}

	if (instancing) {
		for (auto description : InstanceBufferObject::getAttributeDescriptions()) {
			attributeDescriptions.push_back(description);
		}
	}
	return attributeDescriptions;
}

void RenderEntityFactory::CreateGraphicsPipeline(RenderDataVulkanContext& renderDataVulkanContext, RenderDataSingular* renderDataSingular, std::string vert, std::string frag, VkPipeline& pipeline, VkPipelineLayout& pipelineLayout, bool instancing) {
	auto vertShaderCode = ReadFile(vert);
	auto fragShaderCode = ReadFile(frag);
	auto vertShaderModule = CreateShaderModule(vertShaderCode, renderDataVulkanContext.device);
	auto fragShaderModule = CreateShaderModule(fragShaderCode, renderDataVulkanContext.device);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	auto bindingDescriptions = CreateVertexBindingDescription(instancing);
	auto attributeDescriptions = CreateVertexAttributeDescription(instancing);
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)renderDataVulkanContext.swapChainExtent.width;
	viewport.height = (float)renderDataVulkanContext.swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = renderDataVulkanContext.swapChainExtent;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	//rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	//rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	VkPipelineColorBlendAttachmentState colorBlendAttachment;
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &renderDataSingular->descriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	if (vkCreatePipelineLayout(renderDataVulkanContext.device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr; // Optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = nullptr; // Optional
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderDataVulkanContext.renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	if (vkCreateGraphicsPipelines(renderDataVulkanContext.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	vkDestroyShaderModule(renderDataVulkanContext.device, fragShaderModule, nullptr);
	vkDestroyShaderModule(renderDataVulkanContext.device, vertShaderModule, nullptr);
}


void RenderEntityFactory::CreateInstanceBuffer(RenderDataVulkanContext &renderDataVulkanContext, RenderDataInstanced* renderDataInstanced) {
	renderDataInstanced->instanceBufferObjects = new InstanceBufferObject[renderDataInstanced->objectCount];
	renderDataInstanced->instanceBuffers.resize(renderDataVulkanContext.swapChainImages.size());
	renderDataInstanced->instanceMemory.resize(renderDataVulkanContext.swapChainImages.size());

	for (int j = 0; j < renderDataInstanced->objectCount; ++j) {
		renderDataInstanced->instanceBufferObjects[j].MVP = glm::mat4(1);
	}

	for (int i = 0; i < renderDataVulkanContext.swapChainImages.size(); ++i) {
		VkDeviceSize bufferSize = sizeof(InstanceBufferObject) * renderDataInstanced->objectCount;
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		CreateBuffer(renderDataVulkanContext, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(renderDataVulkanContext.device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, renderDataInstanced->instanceBufferObjects, (size_t)bufferSize);
		vkUnmapMemory(renderDataVulkanContext.device, stagingBufferMemory);

		CreateBuffer(renderDataVulkanContext, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, renderDataInstanced->instanceBuffers[i], renderDataInstanced->instanceMemory[i]);
		CopyBuffer(renderDataVulkanContext, stagingBuffer, renderDataInstanced->instanceBuffers[i], bufferSize);

		vkDestroyBuffer(renderDataVulkanContext.device, stagingBuffer, nullptr);
		vkFreeMemory(renderDataVulkanContext.device, stagingBufferMemory, nullptr);
	}
}

void RenderEntityFactory::CreateBuffer(RenderDataVulkanContext &renderDataVulkanContext, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(renderDataVulkanContext.device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(renderDataVulkanContext.device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(renderDataVulkanContext, memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(renderDataVulkanContext.device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(renderDataVulkanContext.device, buffer, bufferMemory, 0);
}

uint32_t RenderEntityFactory::FindMemoryType(RenderDataVulkanContext &renderDataVulkanContext, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(renderDataVulkanContext.physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

void RenderEntityFactory::CopyBuffer(RenderDataVulkanContext &renderDataVulkanContext, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = renderDataVulkanContext.commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(renderDataVulkanContext.device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(renderDataVulkanContext.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(renderDataVulkanContext.graphicsQueue);

	vkFreeCommandBuffers(renderDataVulkanContext.device, renderDataVulkanContext.commandPool, 1, &commandBuffer);
}

void RenderEntityFactory::CreateDescriptorSetLayout(RenderDataVulkanContext &renderDataVulkanContext, RenderDataSingular* renderDataSingular) {
	VkDescriptorSetLayoutBinding uboLayoutBinding;
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;

	if (vkCreateDescriptorSetLayout(renderDataVulkanContext.device, &layoutInfo, nullptr, &renderDataSingular->descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void RenderEntityFactory::CreateUniformBuffers(RenderDataVulkanContext &renderDataVulkanContext, RenderDataSingular* renderDataSingular) {
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	renderDataSingular->uniformBuffers.resize(renderDataVulkanContext.swapChainImages.size());
	renderDataSingular->uniformBuffersMemory.resize(renderDataVulkanContext.swapChainImages.size());

	for (size_t i = 0; i < renderDataVulkanContext.swapChainImages.size(); i++) {
		CreateBuffer(renderDataVulkanContext, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, renderDataSingular->uniformBuffers[i], renderDataSingular->uniformBuffersMemory[i]);
	}
}

void RenderEntityFactory::CreateDescriptorPool(RenderDataVulkanContext &renderDataVulkanContext, RenderDataSingular* renderDataSingular) {
	VkDescriptorPoolSize poolSize;
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = static_cast<uint32_t>(renderDataVulkanContext.swapChainImages.size());

	VkDescriptorPoolCreateInfo poolInfo;
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = static_cast<uint32_t>(renderDataVulkanContext.swapChainImages.size());

	if (vkCreateDescriptorPool(renderDataVulkanContext.device, &poolInfo, nullptr, &renderDataSingular->descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void RenderEntityFactory::CreateDescriptorSets(RenderDataVulkanContext &renderDataVulkanContext, RenderDataSingular* renderDataSingular) {
	std::vector<VkDescriptorSetLayout> layouts(renderDataVulkanContext.swapChainImages.size(), renderDataSingular->descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = renderDataSingular->descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(renderDataVulkanContext.swapChainImages.size());
	allocInfo.pSetLayouts = layouts.data();

	renderDataSingular->descriptorSets.resize(renderDataVulkanContext.swapChainImages.size());
	if (vkAllocateDescriptorSets(renderDataVulkanContext.device, &allocInfo, renderDataSingular->descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < renderDataVulkanContext.swapChainImages.size(); i++) {
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = renderDataSingular->uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = renderDataSingular->descriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pImageInfo = nullptr; // Optional
		descriptorWrite.pTexelBufferView = nullptr; // Optional

		vkUpdateDescriptorSets(renderDataVulkanContext.device, 1, &descriptorWrite, 0, nullptr);
	}
}