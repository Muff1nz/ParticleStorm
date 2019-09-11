#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <fstream>

#include "RenderEntityFactory.h"
#include "Vertex.h"
#include "InstanceBufferObject.h"
#include "VulkanAllocator.h"
#include "RenderEntityCreateInfo.h"

RenderEntityFactory::RenderEntityFactory() = default;

RenderEntityFactory::~RenderEntityFactory() = default;

RenderEntity* RenderEntityFactory::CreateRenderEntity(RenderEntityCreateInfo& createInfo, RenderDataVulkanContext* renderDataVulkanContext, VulkanAllocator* vulkanAllocator, RenderTransform* transform, bool debugEntity) {		
	RenderDataCore* renderDataCore = new RenderDataCore();
	renderDataCore->transform = *transform;
	renderDataCore->renderMode = createInfo.renderMode;
	renderDataCore->vertexBuffer = createInfo.vertexBuffer;
	renderDataCore->indexBuffer = createInfo.indexBuffer;
	renderDataCore->indexCount = createInfo.indexCount;

	if (transform->objectCount > 1) {
		RenderDataInstanced* renderDataInstanced = new RenderDataInstanced();
		renderDataInstanced->instanceCount = transform->objectCount;
		CreateGraphicsPipeline(*renderDataVulkanContext, nullptr, createInfo.vertexShader, createInfo.fragmentShader, renderDataCore->pipeline, renderDataCore->pipelineLayout, createInfo.renderMode, true);
		CreateInstanceBuffer(vulkanAllocator, *renderDataVulkanContext, renderDataInstanced);
		return new RenderEntity(renderDataVulkanContext, renderDataCore, nullptr, renderDataInstanced, debugEntity);
	}

	RenderDataSingular* renderDataSingular = new RenderDataSingular();
	CreateDescriptorSetLayout(*renderDataVulkanContext, renderDataSingular);
	CreateGraphicsPipeline(*renderDataVulkanContext, renderDataSingular, createInfo.vertexShader, createInfo.fragmentShader, renderDataCore->pipeline, renderDataCore->pipelineLayout, createInfo.renderMode, false);
	CreateUniformBuffers(vulkanAllocator, *renderDataVulkanContext, renderDataSingular);
	CreateDescriptorPool(*renderDataVulkanContext, renderDataSingular);
	CreateDescriptorSets(*renderDataVulkanContext, renderDataSingular);
	return new RenderEntity(renderDataVulkanContext, renderDataCore, renderDataSingular, nullptr, debugEntity);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderEntityFactory::CreateGraphicsPipeline(RenderDataVulkanContext& renderDataVulkanContext, RenderDataSingular* renderDataSingular, std::string vert, std::string frag, VkPipeline& pipeline, VkPipelineLayout& pipelineLayout, RenderMode renderMode, bool instancing) {
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
	inputAssembly.topology = renderMode == Triangles ? VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST : VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
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
	rasterizer.polygonMode = renderMode == Triangles ? VK_POLYGON_MODE_FILL : VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = Triangles ? 1.0f : 2.0f;
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
	pipelineLayoutInfo.setLayoutCount = renderDataSingular != nullptr ? 1 : 0;
	pipelineLayoutInfo.pSetLayouts = renderDataSingular != nullptr ? &renderDataSingular->descriptorSetLayout : nullptr;
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

std::vector<VkVertexInputBindingDescription> RenderEntityFactory::CreateVertexBindingDescription(bool instancing) {
	std::vector<VkVertexInputBindingDescription> bindingDescriptions = {};
	bindingDescriptions.push_back(Vertex::getBindingDescription());
	if (instancing)
		bindingDescriptions.push_back(InstanceBufferObject::getBindingDescription());
	return bindingDescriptions;
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void RenderEntityFactory::CreateInstanceBuffer(VulkanAllocator* vulkanAllocator, RenderDataVulkanContext &renderDataVulkanContext, RenderDataInstanced* renderDataInstanced) {
	renderDataInstanced->instanceBufferObjects = new InstanceBufferObject[renderDataInstanced->instanceCount];
	renderDataInstanced->instanceBuffers.resize(renderDataVulkanContext.swapChainImages.size());
	renderDataInstanced->instanceMemory.resize(renderDataVulkanContext.swapChainImages.size());

	for (int j = 0; j < renderDataInstanced->instanceCount; ++j) {
		renderDataInstanced->instanceBufferObjects[j].MVP = glm::mat4(1);
	}

	for (int i = 0; i < renderDataVulkanContext.swapChainImages.size(); ++i) {
		VkDeviceSize bufferSize = sizeof(InstanceBufferObject) * renderDataInstanced->instanceCount;
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		vulkanAllocator->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(renderDataVulkanContext.device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, renderDataInstanced->instanceBufferObjects, (size_t)bufferSize);
		vkUnmapMemory(renderDataVulkanContext.device, stagingBufferMemory);

		vulkanAllocator->CreateBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, renderDataInstanced->instanceBuffers[i], renderDataInstanced->instanceMemory[i]);
		vulkanAllocator->CopyBuffer(stagingBuffer, renderDataInstanced->instanceBuffers[i], bufferSize);

		vkDestroyBuffer(renderDataVulkanContext.device, stagingBuffer, nullptr);
		vkFreeMemory(renderDataVulkanContext.device, stagingBufferMemory, nullptr);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

void RenderEntityFactory::CreateUniformBuffers(VulkanAllocator* vulkanAllocator, RenderDataVulkanContext &renderDataVulkanContext, RenderDataSingular* renderDataSingular) {
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	renderDataSingular->uniformBuffers.resize(renderDataVulkanContext.swapChainImages.size());
	renderDataSingular->uniformBuffersMemory.resize(renderDataVulkanContext.swapChainImages.size());

	for (size_t i = 0; i < renderDataVulkanContext.swapChainImages.size(); i++) {
		vulkanAllocator->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, renderDataSingular->uniformBuffers[i], renderDataSingular->uniformBuffersMemory[i]);
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