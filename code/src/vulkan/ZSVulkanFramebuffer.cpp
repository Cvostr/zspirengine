#include "../../headers/vulkan/ZSVulkanFramebuffer.hpp"
#include "../../headers/game.h"

extern ZSGAME_DATA* game_data;

Engine::ZSVulkanFramebuffer::ZSVulkanFramebuffer() {
	mLayersCount = 1;
}

Engine::ZSVulkanFramebuffer::ZSVulkanFramebuffer(unsigned int width, unsigned int height) {
	mLayersCount = 1;

	Width = width;
	Height = height;
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

void Engine::ZSVulkanFramebuffer::PushOutputAttachment() {
	Views.push_back(game_data->vk_main->mSwapChain->GetImageViewAtIndex(0));
}

void Engine::ZSVulkanFramebuffer::AddTexture(TextureFormat Format) {
	AddTexture(Width, Height, Format);
}
void Engine::ZSVulkanFramebuffer::AddDepth(unsigned int Layers, TextureFormat Format) {
	AddDepth(Width, Height, Layers, Format);
}

void Engine::ZSVulkanFramebuffer::AddTexture(uint32_t Width, uint32_t Height, TextureFormat Format) {
	vkTexture* Texture = new vkTexture;

	Texture->usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	Texture->aspect = VK_IMAGE_ASPECT_COLOR_BIT;

	Texture->Create(Width, Height, Format);

	textures[mTexturesCount++] = Texture;

	Views.push_back(Texture->GetImageView());
}

void Engine::ZSVulkanFramebuffer::AddDepth(uint32_t Width, uint32_t Height, unsigned int Layers, TextureFormat Format) {
	Depth = true;

	vkTexture* DepthTexture = new vkTexture;

	DepthTexture->usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	DepthTexture->aspect = VK_IMAGE_ASPECT_DEPTH_BIT;

	DepthTexture->Create(Width, Height, Format, Layers);

	depthTexture = DepthTexture;

	Views.push_back(DepthTexture->GetImageView());
}