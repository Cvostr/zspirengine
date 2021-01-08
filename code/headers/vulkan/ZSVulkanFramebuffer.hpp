#pragma once

#include "../render/Framebuffer.hpp"
#include "../vulkan/ZSVulkanRenderPass.hpp"
#include "../render/GpuObject.hpp"
#include <vulkan/vulkan.hpp>
#include <vector>

namespace Engine {

    typedef struct FbAttachment {
        unsigned int Width;
        unsigned int Height;
        unsigned int Layers;
        VkFormat format;
        VkImageUsageFlagBits usage;
        VkImageAspectFlagBits aspect;

        VkImage Image;
        VkDeviceMemory ImageMemory;
        VkImageView ImageView;

        FbAttachment() {
            Layers = 1;
        }
    }FbAttachment;

	class ZSVulkanFramebuffer : public GpuObject {
	private:
		VkFramebuffer mFramebuffer;
        std::vector<FbAttachment> Attachments;
        std::vector<VkImageView> Views;
        unsigned int mLayersCount;
	public:

        VkFramebuffer GetFramebuffer() { return mFramebuffer; }

        VkImageView getImageViewIndex(unsigned int i) { return Views[i]; }
        VkImageView getImageView(FbAttachment* attachment);
        FbAttachment* PushAttachment(unsigned int Width, unsigned int Height, VkFormat format, VkImageUsageFlagBits usage, VkImageAspectFlagBits aspect, unsigned int Layers = 1);
        void PushOutputAttachment();
        void PushDepthAttachment(unsigned int Width, unsigned int Height, unsigned int Layers = 1);
        void SetLayersCount(unsigned int Layers) { mLayersCount = Layers; }
		bool Create(ZSVulkanRenderPass* renderpass);
		ZSVulkanFramebuffer();
        ~ZSVulkanFramebuffer();
	};
}