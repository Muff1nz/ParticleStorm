#include "RenderEntity.h"
#include <ext/matrix_transform.inl>
#include "Camera.h"
#include "Environment.h"

RenderEntity::RenderEntity(RenderDataVulkanContext* renderDataVulkanContext, RenderDataCore* renderDataCore, RenderDataSingular* renderDataSingular, RenderDataInstanced* renderDataInstanced) {
	this->renderDataVulkanContext = renderDataVulkanContext;
	this->renderDataCore = renderDataCore;
	this->renderDataSingular = renderDataSingular;
	this->renderDataInstanced = renderDataInstanced;
}

RenderEntity::~RenderEntity() {
	Dispose();
}

void RenderEntity::Dispose() {
	if (isDisposed)
		return;

	if (renderDataInstanced != nullptr) {
		for (size_t i = 0; i < renderDataVulkanContext->swapChainImages.size(); ++i) {
			vkDestroyBuffer(renderDataVulkanContext->device, renderDataInstanced->instanceBuffers[i], nullptr);
			vkFreeMemory(renderDataVulkanContext->device, renderDataInstanced->instanceMemory[i], nullptr);
		}
	}

	if (renderDataSingular != nullptr) {
		for (size_t i = 0; i < renderDataVulkanContext->swapChainImages.size(); ++i) {
			vkDestroyBuffer(renderDataVulkanContext->device, renderDataSingular->uniformBuffers[i], nullptr);
			vkFreeMemory(renderDataVulkanContext->device, renderDataSingular->uniformBuffersMemory[i], nullptr);
		}
		vkDestroyDescriptorPool(renderDataVulkanContext->device, renderDataSingular->descriptorPool, nullptr);
	}

	if (renderDataCore != nullptr) {
		vkDestroyPipeline(renderDataVulkanContext->device, renderDataCore->pipeline, nullptr);
		vkDestroyPipelineLayout(renderDataVulkanContext->device, renderDataCore->pipelineLayout, nullptr);
	}

	if (renderDataSingular != nullptr) {
		vkDestroyDescriptorSetLayout(renderDataVulkanContext->device, renderDataSingular->descriptorSetLayout, nullptr);
	}

	isDisposed = true;
}

void RenderEntity::UpdateBuffers(uint32_t imageIndex, Environment* environment) const {
	if (renderDataSingular != nullptr) {
		UpdateUniformBuffer(imageIndex, environment);
	}

	if (renderDataInstanced != nullptr) {
		UpdateInstanceBuffer(imageIndex, environment);
	}
}

void RenderEntity::BindToCommandPool(std::vector<VkCommandBuffer> &commandBuffers, VkBuffer &quadVertexBuffer, VkBuffer &quadIndexBuffer, const std::vector<uint16_t> &indices, int index) const {
	VkDeviceSize offsets[] = { 0 };
	VkBuffer vertexBuffers[] = { quadVertexBuffer };

	if (renderDataSingular != nullptr) {
		vkCmdBindPipeline(commandBuffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS, renderDataCore->pipeline);
		vkCmdBindVertexBuffers(commandBuffers[index], 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffers[index], quadIndexBuffer, 0, VK_INDEX_TYPE_UINT16);
		vkCmdBindDescriptorSets(commandBuffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS, renderDataCore->pipelineLayout, 0, 1, &renderDataSingular->descriptorSets[index], 0, nullptr);
		vkCmdDrawIndexed(commandBuffers[index], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
	}

	if (renderDataInstanced != nullptr) {
		VkBuffer instanceBuffer[] = { renderDataInstanced->instanceBuffers[index] };
		vkCmdBindPipeline(commandBuffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS, renderDataCore->pipeline);
		vkCmdBindVertexBuffers(commandBuffers[index], 0, 1, vertexBuffers, offsets);
		vkCmdBindVertexBuffers(commandBuffers[index], 1, 1, instanceBuffer, offsets);
		vkCmdBindIndexBuffer(commandBuffers[index], quadIndexBuffer, 0, VK_INDEX_TYPE_UINT16);
		vkCmdDrawIndexed(commandBuffers[index], static_cast<uint32_t>(indices.size()), static_cast<uint32_t>(renderDataInstanced->objectCount), 0, 0, 0);
	}
}

void RenderEntity::UpdateInstanceBuffer(uint32_t imageIndex, Environment* environment) const {
	glm::vec2* particles = renderDataCore->transform.pos;
	glm::mat4 projView = environment->camera.GetProj() * environment->camera.GetView();

	for (int i = 0; i < renderDataInstanced->objectCount; ++i) {
		glm::vec2 pos = particles[i];
		glm::mat4 model = translate(glm::mat4(1), glm::vec3(pos, 0)) * scale(glm::mat4(1), { renderDataCore->transform.scale, 1 });
		renderDataInstanced->instanceBufferObjects[i].MVP = projView * model;
	}

	auto size = sizeof(InstanceBufferObject) * renderDataInstanced->objectCount;

	void* data;
	vkMapMemory(renderDataVulkanContext->device, renderDataInstanced->instanceMemory[imageIndex], 0, size, 0, &data);
	memcpy(data, renderDataInstanced->instanceBufferObjects, size);
	vkUnmapMemory(renderDataVulkanContext->device, renderDataInstanced->instanceMemory[imageIndex]);
}

void RenderEntity::UpdateUniformBuffer(uint32_t imageIndex, Environment* environment) const {
	UniformBufferObject ubo = {};
	glm::mat4 projView = environment->camera.GetProj() * environment->camera.GetView();
	glm::mat4 model = translate(glm::mat4(1), glm::vec3(*renderDataCore->transform.pos, 0)) * scale(glm::mat4(1), { renderDataCore->transform.scale, 1 });
	ubo.MVP = projView * model;

	void* data;
	vkMapMemory(renderDataVulkanContext->device, renderDataSingular->uniformBuffersMemory[imageIndex], 0, sizeof(UniformBufferObject), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(renderDataVulkanContext->device, renderDataSingular->uniformBuffersMemory[imageIndex]);
}
