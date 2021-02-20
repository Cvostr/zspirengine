#include <vulkan/ZSVulkanFramebuffer.hpp>
#include <game.h>

extern ZSGAME_DATA* game_data;

Engine::ZSVulkanFramebuffer::ZSVulkanFramebuffer() {
	mLayersCount = 1;

	Width = 640;
	Height = 480;
}

Engine::ZSVulkanFramebuffer::ZSVulkanFramebuffer(unsigned int width, unsigned int height) {
	mLayersCount = 1;

	Width = width;
	Height = height;
}

Engine::ZSVulkanFramebuffer::~ZSVulkanFramebuffer() {
	Destroy();
}

bool Engine::ZSVulkanFramebuffer::Create(ZSVulkanRenderPass* renderpass) {
	if (!mCreated) {
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderpass->GetRenderPass();
		framebufferInfo.attachmentCount = static_cast<uint32_t>(Views.size());
		framebufferInfo.pAttachments = Views.data();
		framebufferInfo.width = Width;
		framebufferInfo.height = Height;
		framebufferInfo.layers = mLayersCount;

		if (vkCreateFramebuffer(game_data->vk_main->mDevice->getVkDevice(), &framebufferInfo, nullptr, &mFramebuffer) != VK_SUCCESS) {
			return false;
		}

		mPickedRenderPass = renderpass;

		mCreated = true;

	}
	return true;
}

void Engine::ZSVulkanFramebuffer::Create() {
	if (Usage == FB_USAGE_DEFAULT)
		return;

	ZSVulkanRenderPass* RenderPass = nullptr;

	switch (Usage) {
	case FB_USAGE_GBUFFER_PASS: {
		RenderPass = game_data->vk_main->mGBufferRenderPass;
		break;
	}
	case FB_USAGE_DEFERRED_PASS: {
		RenderPass = game_data->vk_main->mDefferedRenderPass;
		break;
	}
	}

	Create(RenderPass);
}

void Engine::ZSVulkanFramebuffer::Destroy() {
	if (mCreated) {
		vkDestroyFramebuffer(game_data->vk_main->mDevice->getVkDevice(), mFramebuffer, nullptr);

		for (unsigned int t = 0; t < mTexturesCount; t++) {
			textures[t]->Destroy();
			delete textures[t];
		}

		if (Depth) {
			depthTexture->Destroy();
			delete depthTexture;
		}

		mTexturesCount = 0;
		mCreated = false;
	}
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
	if (mTexturesCount == MAX_RENDERER_ATTACHMENT_COUNT) 
		return;

	if (Format == TextureFormat::FORMAT_RGB16F)
		Format = TextureFormat::FORMAT_RGBA16F;

	vkTexture* Texture = new vkTexture;

	Texture->usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	Texture->aspect = VK_IMAGE_ASPECT_COLOR_BIT;
	Texture->SetRenderTargetFlag(true);

	Texture->Create(Width, Height, Format);
	//Push new texture
	AddTexture(Texture);
}

void Engine::ZSVulkanFramebuffer::AddDepth(uint32_t Width, uint32_t Height, unsigned int Layers, TextureFormat Format) {
	if (!Depth) {
		Depth = true;

		vkTexture* DepthTexture = new vkTexture;
		DepthTexture->SetRenderTargetFlag(true);
		DepthTexture->usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		DepthTexture->aspect = VK_IMAGE_ASPECT_DEPTH_BIT;

		DepthTexture->Create(Width, Height, Format, Layers);

		depthTexture = DepthTexture;

		Views.push_back(DepthTexture->GetImageView());
	}
}

void Engine::ZSVulkanFramebuffer::AddTexture(Texture* Texture) {
	if (mTexturesCount == MAX_RENDERER_ATTACHMENT_COUNT || !Texture->IsRenderTarget()) return;

	textures[mTexturesCount++] = Texture;

	Views.push_back(((vkTexture*)Texture)->GetImageView());
}

void Engine::ZSVulkanFramebuffer::SetSize(uint32_t Width, uint32_t Height) {
	Framebuffer::SetSize(Width, Height);

	//Resize all textures
	for (unsigned int t = 0; t < mTexturesCount; t++) {
		textures[t]->Resize(Width, Height);
	}
	//Resize depth, if exists
	if (Depth) {
		depthTexture->Resize(Width, Height);
	}

	//if framebuffer already created
	if (mCreated) {
		//Destroy current framebuffer
		vkDestroyFramebuffer(game_data->vk_main->mDevice->getVkDevice(), mFramebuffer, nullptr);

		mCreated = false;
		Create(mPickedRenderPass);
	}
}