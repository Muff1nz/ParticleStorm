#include "RenderEntity.h"
#include <ext/matrix_transform.inl>
#include "Camera.h"
#include "Environment.h"

RenderEntity::RenderEntity(VulkanContext* renderDataVulkanContext, RenderDataCore* renderDataCore, RenderDataUniform* renderDataSingular, RenderDataInstanced* renderDataInstanced, RenderEntityMeta* renderEntityMeta, bool debugEntity) {
	this->renderEntityMeta = renderEntityMeta;
	this->renderDataVulkanContext = renderDataVulkanContext;
	this->renderDataCore = renderDataCore;
	this->renderDataUniform = renderDataSingular;
	this->renderDataInstanced = renderDataInstanced;
	this->debugEntity = debugEntity;

	isDisposed = false;
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
		delete renderDataInstanced;
	}

	if (renderDataUniform != nullptr) {
		//Texture
		vkDestroySampler(renderDataVulkanContext->device, renderDataUniform->textureSampler, nullptr);
		vkDestroyImageView(renderDataVulkanContext->device, renderDataUniform->textureImageView, nullptr);
		vkDestroyImage(renderDataVulkanContext->device, renderDataUniform->textureImage, nullptr);
		vkFreeMemory(renderDataVulkanContext->device, renderDataUniform->textureImageMemory, nullptr);

		if (renderDataInstanced != nullptr) {
			for (size_t i = 0; i < renderDataVulkanContext->swapChainImages.size(); ++i) {
				vkDestroyBuffer(renderDataVulkanContext->device, renderDataUniform->uniformBuffers[i], nullptr);
				vkFreeMemory(renderDataVulkanContext->device, renderDataUniform->uniformBuffersMemory[i], nullptr);
			}
		}

		vkDestroyDescriptorPool(renderDataVulkanContext->device, renderDataUniform->descriptorPool, nullptr);
	}

	if (renderDataCore != nullptr) {
		vkDestroyPipeline(renderDataVulkanContext->device, renderDataCore->pipeline, nullptr);
		vkDestroyPipelineLayout(renderDataVulkanContext->device, renderDataCore->pipelineLayout, nullptr);
		delete renderDataCore;
	}

	if (renderDataUniform != nullptr) {
		vkDestroyDescriptorSetLayout(renderDataVulkanContext->device, renderDataUniform->descriptorSetLayout, nullptr);
		delete renderDataUniform;
	}

	delete renderEntityMeta;


	isDisposed = true;
}

void RenderEntity::DisposePipeline() const {
	if (renderDataCore != nullptr) {
		vkDestroyPipeline(renderDataVulkanContext->device, renderDataCore->pipeline, nullptr);
		vkDestroyPipelineLayout(renderDataVulkanContext->device, renderDataCore->pipelineLayout, nullptr);
	}
}

bool RenderEntity::IsDebugEntity() const {
	return debugEntity;
}

void RenderEntity::UpdateBuffers(uint32_t imageIndex, Camera* camera) const {
	if (renderDataUniform != nullptr && renderDataInstanced == nullptr) {
		UpdateUniformBuffer(imageIndex, camera);
	}

	if (renderDataInstanced != nullptr) {
		UpdateInstanceBuffer(imageIndex, camera);
	}
}

void RenderEntity::BindToCommandPool(std::vector<VkCommandBuffer> &commandBuffers, int index) const {
	VkDeviceSize offsets[] = { 0 };
	VkBuffer vertexBuffers[] = { renderDataCore->vertexBuffer };

	//if (renderDataUniform != nullptr) {
	//	vkCmdBindPipeline(commandBuffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS, renderDataCore->pipeline);
	//	vkCmdBindVertexBuffers(commandBuffers[index], 0, 1, vertexBuffers, offsets);
	//	vkCmdBindIndexBuffer(commandBuffers[index], renderDataCore->indexBuffer, 0, VK_INDEX_TYPE_UINT16);
	//	vkCmdBindDescriptorSets(commandBuffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS, renderDataCore->pipelineLayout, 0, 1, &renderDataUniform->descriptorSets[index], 0, nullptr);
	//	vkCmdDrawIndexed(commandBuffers[index], renderDataCore->indexCount, 1, 0, 0, 0);
	//}

	//if (renderDataInstanced != nullptr) {
	//	VkBuffer instanceBuffer[] = { renderDataInstanced->instanceBuffers[index] };
	//	vkCmdBindPipeline(commandBuffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS, renderDataCore->pipeline);
	//	vkCmdBindVertexBuffers(commandBuffers[index], 0, 1, vertexBuffers, offsets);
	//	vkCmdBindVertexBuffers(commandBuffers[index], 1, 1, instanceBuffer, offsets);
	//	vkCmdBindIndexBuffer(commandBuffers[index], renderDataCore->indexBuffer, 0, VK_INDEX_TYPE_UINT16);
	//	vkCmdDrawIndexed(commandBuffers[index], renderDataCore->indexCount, static_cast<uint32_t>(renderDataInstanced->instanceCount), 0, 0, 0);
	//}

	if (renderDataUniform != nullptr && renderDataInstanced != nullptr) {
		VkBuffer instanceBuffer[] = { renderDataInstanced->instanceBuffers[index] };
		vkCmdBindPipeline(commandBuffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS, renderDataCore->pipeline);
		vkCmdBindVertexBuffers(commandBuffers[index], 0, 1, vertexBuffers, offsets);
		vkCmdBindVertexBuffers(commandBuffers[index], 1, 1, instanceBuffer, offsets);
		vkCmdBindIndexBuffer(commandBuffers[index], renderDataCore->indexBuffer, 0, VK_INDEX_TYPE_UINT16);
		vkCmdBindDescriptorSets(commandBuffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS, renderDataCore->pipelineLayout, 0, 1, &renderDataUniform->descriptorSets[index], 0, nullptr);
		vkCmdDrawIndexed(commandBuffers[index], renderDataCore->indexCount, static_cast<uint32_t>(renderDataInstanced->instanceCount), 0, 0, 0);
	}else if (renderDataUniform != nullptr) {
		vkCmdBindPipeline(commandBuffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS, renderDataCore->pipeline);
		vkCmdBindVertexBuffers(commandBuffers[index], 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffers[index], renderDataCore->indexBuffer, 0, VK_INDEX_TYPE_UINT16);
		vkCmdBindDescriptorSets(commandBuffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS, renderDataCore->pipelineLayout, 0, 1, &renderDataUniform->descriptorSets[index], 0, nullptr);
		vkCmdDrawIndexed(commandBuffers[index], renderDataCore->indexCount, 1, 0, 0, 0);
	} else if (renderDataInstanced != nullptr) {
		VkBuffer instanceBuffer[] = { renderDataInstanced->instanceBuffers[index] };
		vkCmdBindPipeline(commandBuffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS, renderDataCore->pipeline);
		vkCmdBindVertexBuffers(commandBuffers[index], 0, 1, vertexBuffers, offsets);
		vkCmdBindVertexBuffers(commandBuffers[index], 1, 1, instanceBuffer, offsets);
		vkCmdBindIndexBuffer(commandBuffers[index], renderDataCore->indexBuffer, 0, VK_INDEX_TYPE_UINT16);
		vkCmdDrawIndexed(commandBuffers[index], renderDataCore->indexCount, static_cast<uint32_t>(renderDataInstanced->instanceCount), 0, 0, 0);
	}
}

void RenderEntity::UpdateInstanceBuffer(uint32_t imageIndex, Camera* camera) const {
	glm::vec2* pos = renderDataCore->transform.pos;
	glm::vec2* scale = renderDataCore->transform.scale;
	glm::mat4 projView = camera->GetProj() * camera->GetView();

	if (!renderDataCore->transform.staticScale) {
		for (int i = 0; i < renderDataInstanced->instanceCount; ++i) {
			glm::mat4 model = glm::translate(glm::mat4(1), glm::vec3(pos[i], 0)) * glm::scale(glm::mat4(1), glm::vec3(scale[i], 1));
			renderDataInstanced->instanceBufferObjects[i].MVP = projView * model;
		}
	} else {
		for (int i = 0; i < renderDataInstanced->instanceCount; ++i) {
			glm::mat4 model = glm::translate(glm::mat4(1), glm::vec3(pos[i], 0)) * glm::scale(glm::mat4(1), glm::vec3(*scale, 1));
			renderDataInstanced->instanceBufferObjects[i].MVP = projView * model;
		}
	}

	auto size = sizeof(InstanceBufferObject) * renderDataInstanced->instanceCount;

	void* data;
	vkMapMemory(renderDataVulkanContext->device, renderDataInstanced->instanceMemory[imageIndex], 0, size, 0, &data);
	memcpy(data, renderDataInstanced->instanceBufferObjects, size);
	vkUnmapMemory(renderDataVulkanContext->device, renderDataInstanced->instanceMemory[imageIndex]);
}

void RenderEntity::UpdateUniformBuffer(uint32_t imageIndex, Camera* camera) const {
	UniformBufferObject ubo = {};
	glm::mat4 projView = camera->GetProj() * camera->GetView();
	glm::mat4 model = glm::translate(glm::mat4(1), glm::vec3(*renderDataCore->transform.pos, 0)) * glm::scale(glm::mat4(1), glm::vec3(*renderDataCore->transform.scale, 1));
	ubo.MVP = projView * model;

	void* data;
	vkMapMemory(renderDataVulkanContext->device, renderDataUniform->uniformBuffersMemory[imageIndex], 0, sizeof(UniformBufferObject), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(renderDataVulkanContext->device, renderDataUniform->uniformBuffersMemory[imageIndex]);
}
