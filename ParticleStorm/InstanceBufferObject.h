#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <array>


struct InstanceBufferObject {
	glm::mat4 MVP;

	static VkVertexInputBindingDescription GetBindingDescription() {
		VkVertexInputBindingDescription bindingDescription;
		bindingDescription.binding = 1;
		bindingDescription.stride = sizeof(InstanceBufferObject);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 4> GetAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions = {};
		for (int i = 0; i < 4; ++i) {
			attributeDescriptions[i].binding = 1;
			attributeDescriptions[i].location = i + 3; //Vertex takes locations 1-2
			attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			attributeDescriptions[i].offset = sizeof(glm::vec4) * i;
		}
		return attributeDescriptions;
	}
};
