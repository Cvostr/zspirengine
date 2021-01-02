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
        VkFormat format;
        VkImageUsageFlagBits usage;
        VkImageAspectFlagBits aspect;

        VkImage Image;
        VkDeviceMemory ImageMemory;
        VkImageView ImageView;
    }FbAttachment;

	class ZSVulkanFramebuffer : public GpuObject {
	private:
		VkFramebuffer mFramebuffer;
        std::vector<FbAttachment> Attachments;
        std::vector<VkImageView> Views;
	public:

        VkFramebuffer GetFramebuffer() { return mFramebuffer; }

        VkImageView getImageView(FbAttachment* attachment);
        FbAttachment* PushAttachment(unsigned int Width, unsigned int Height, VkFormat format, VkImageUsageFlagBits usage, VkImageAspectFlagBits aspect);
        void PushOutputAttachment();
        void PushDepthAttachment(unsigned int Width, unsigned int Height);

		bool Create(ZSVulkanRenderPass* renderpass);
		ZSVulkanFramebuffer();
        ~ZSVulkanFramebuffer();
	};
}