#include "../../headers/vulkan/ZSVulkanFramebuffer.hpp"
#include "../../headers/game.h"

extern ZSGAME_DATA* game_data;

Engine::ZSVulkanFramebuffer::ZSVulkanFramebuffer() {

}

bool Engine::ZSVulkanFramebuffer::Create(ZSVulkanPipeline* pipeline) {
	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = pipeline->GetRenderPass();
	framebufferInfo.attachmentCount = Views.size();
	framebufferInfo.pAttachments = Views.data();
	framebufferInfo.width = game_data->vk_main->mSwapChain->GetExtent().width;
	framebufferInfo.height = game_data->vk_main->mSwapChain->GetExtent().height;
	framebufferInfo.layers = 1;

	if (vkCreateFramebuffer(game_data->vk_main->mDevice->getVkDevice(), &framebufferInfo, nullptr, &mFramebuffer) != VK_SUCCESS) {
		return false;
	}
	return true;
}

void Engine::ZSVulkanFramebuffer::PushAttachment(unsigned int Width, unsigned int Height,
	VkFormat format,
	VkImageUsageFlagBits usage,
	VkImageAspectFlagBits aspect) 
{
	FbAttachment Attachment = {};
	Attachment.Width = Width;
	Attachment.Height = Height;
	Attachment.format = format;
	Attachment.usage = usage;
	Attachment.aspect = aspect;

	this->Attachments.push_back(Attachment);
}
void Engine::ZSVulkanFramebuffer::PushOutputAttachment() {
	//PushAttachment();
	Views.push_back(game_data->vk_main->mSwapChain->GetImageViewAtIndex(0));
}
void Engine::ZSVulkanFramebuffer::PushDepthAttachment(unsigned int Width, unsigned int Height) {

}