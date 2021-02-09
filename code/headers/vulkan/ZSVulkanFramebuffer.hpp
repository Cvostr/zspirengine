#pragma once

#include "../render/Framebuffer.hpp"
#include "../vulkan/ZSVulkanRenderPass.hpp"
#include "../render/Texture.h"
#include <vulkan/vulkan.hpp>
#include <vector>

namespace Engine {

	class ZSVulkanFramebuffer : public Framebuffer {
	private:
		VkFramebuffer mFramebuffer;
        std::vector<VkImageView> Views;
        unsigned int mLayersCount;
	public:

        VkFramebuffer GetFramebuffer() { return mFramebuffer; }

        VkImageView getImageViewIndex(unsigned int i) { return Views[i]; }
        void PushOutputAttachment();
        void SetLayersCount(unsigned int Layers) { mLayersCount = Layers; }
		bool Create(ZSVulkanRenderPass* renderpass);
        void Destroy();

        void AddTexture(TextureFormat Format = TextureFormat::FORMAT_RGBA);
        void AddDepth(unsigned int Layers = 1, TextureFormat Format = FORMAT_DEPTH_24_STENCIL_8);

        void AddTexture(uint32_t Width, uint32_t Height, TextureFormat Format);
        void AddDepth(uint32_t Width, uint32_t Height, unsigned int Layers = 1, TextureFormat Format = FORMAT_DEPTH_24_STENCIL_8);
        //Push Texture to framebuffer
        void AddTexture(Texture* Texture);

        void SetSize(uint32_t Width, uint32_t Height);

		ZSVulkanFramebuffer();
        ZSVulkanFramebuffer(unsigned int width, unsigned int height);
        ~ZSVulkanFramebuffer();
	};
}