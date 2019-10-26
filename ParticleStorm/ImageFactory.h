#pragma once
#include "VulkanContext.h"
#include "RenderDataSingular.h"
#include "VulkanAllocator.h"
#include "PsTuple.h"

class ImageFactory {
public:
	ImageFactory(VulkanContext* vulkanContext);
	~ImageFactory();
	
	PsTuple<VkImage, VkDeviceMemory> CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) const;
	PsTuple<VkImage, VkDeviceMemory> CreateTextureImage(std::string texturePath);
	VkImageView CreateImageView(VkImage image, VkFormat format) const;
	VkSampler CreateTextureSampler() const;

private:
	VulkanContext* vulkanContext;
	VulkanAllocator* vulkanAllocator;

	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) const;
	void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) const;
};

