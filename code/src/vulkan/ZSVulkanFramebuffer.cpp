#include "../../headers/vulkan/ZSVulkanFramebuffer.hpp"
#include "../../headers/game.h"

extern ZSGAME_DATA* game_data;

Engine::ZSVulkanFramebuffer::ZSVulkanFramebuffer() {
	mLayersCount = 1;
}

Engine::ZSVulkanFramebuffer::~ZSVulkanFramebuffer() {
	if (mCreated)
		vkDestroyFramebuffer(game_data->vk_main->mDevice->getVkDevice(), mFramebuffer, nullptr);
}

bool Engine::ZSVulkanFramebuffer::Create(ZSVulkanRenderPass* renderpass) {
	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderpass->GetRenderPass();
	framebufferInfo.attachmentCount = static_cast<uint32_t>(Views.size());
	framebufferInfo.pAttachments = Views.data();
	framebufferInfo.width = renderpass->GetClearExtent().width;
	framebufferInfo.height = renderpass->GetClearExtent().height;
	framebufferInfo.layers = mLayersCount;
	

	if (vkCreateFramebuffer(game_data->vk_main->mDevice->getVkDevice(), &framebufferInfo, nullptr, &mFramebuffer) != VK_SUCCESS) {
		return false;
	}

	mCreated = true;

	return true;
}

Engine::FbAttachment* Engine::ZSVulkanFramebuffer::PushAttachment(unsigned int Width, unsigned int Height,
	VkFormat format,
	VkImageUsageFlagBits usage,
	VkImageAspectFlagBits aspect, unsigned int Layers)
{
	FbAttachment Attachment = {};
	Attachment.Width = Width;
	Attachment.Height = Height;
	Attachment.format = format;
	Attachment.usage = usage;
	Attachment.aspect = aspect;
	Attachment.Layers = Layers;

	this->Attachments.push_back(Attachment);


	VkImageView view = getImageView(&Attachments[Attachments.size() - 1]);
	Views.push_back(view);

	return &Attachments[Attachments.size() - 1];
}
void Engine::ZSVulkanFramebuffer::PushOutputAttachment() {
	Views.push_back(game_data->vk_main->mSwapChain->GetImageViewAtIndex(0));
}
void Engine::ZSVulkanFramebuffer::PushDepthAttachment(unsigned int Width, unsigned int Height, unsigned int Layers) {
	FbAttachment* att = PushAttachment(Width, Height, VK_FORMAT_D24_UNORM_S8_UINT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, Layers);
}

VkImageView Engine::ZSVulkanFramebuffer::getImageView(FbAttachment* attachment) {

	VkImageLayout imageLayout;

	if (attachment->usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
	{
		imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}
	if (attachment->usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
	{
		imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.flags = 0; // Optional
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = static_cast<uint32_t>(attachment->Width);
	imageInfo.extent.height = static_cast<uint32_t>(attachment->Height);
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = attachment->Layers;
	
	imageInfo.format = attachment->format;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = imageLayout;
	imageInfo.usage = attachment->usage | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;


	if (vkCreateImage(game_data->vk_main->mDevice->getVkDevice(), &imageInfo, nullptr, &attachment->Image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}
	//Get Memory Requirements
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(game_data->vk_main->mDevice->getVkDevice(), attachment->Image, &memRequirements);
	//Get Memory Properties
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(game_data->vk_main->mDevice->getPhysicalDevice(), &memProperties);

	unsigned int memoryType = 0;
	VkMemoryPropertyFlags flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((memRequirements.memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & flags) == flags) {
			memoryType = i;
		}
	}

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = memoryType;
	//Allocate Memory
	if (vkAllocateMemory(game_data->vk_main->mDevice->getVkDevice(), &allocInfo, nullptr, &attachment->ImageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(game_data->vk_main->mDevice->getVkDevice(), attachment->Image, attachment->ImageMemory, 0);

	VkImageViewType ImageViewType = VK_IMAGE_VIEW_TYPE_2D;
	if (attachment->Layers > 1)
		ImageViewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;

	VkImageViewCreateInfo textureImageViewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	textureImageViewInfo.image = attachment->Image;
	textureImageViewInfo.viewType = ImageViewType;
	textureImageViewInfo.format = attachment->format;
	textureImageViewInfo.subresourceRange.aspectMask = attachment->aspect;
	textureImageViewInfo.subresourceRange.baseMipLevel = 0;
	textureImageViewInfo.subresourceRange.levelCount = 1;
	textureImageViewInfo.subresourceRange.baseArrayLayer = 0;
	textureImageViewInfo.subresourceRange.layerCount = attachment->Layers;
	vkCreateImageView(game_data->vk_main->mDevice->getVkDevice(), &textureImageViewInfo, nullptr, &attachment->ImageView);


	return attachment->ImageView;
}