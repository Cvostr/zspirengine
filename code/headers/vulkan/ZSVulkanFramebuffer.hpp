#pragma once

#include "../render/Framebuffer.hpp"
#include "../vulkan/ZSVulkanPipeline.hpp"
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

	class ZSVulkanFramebuffer {
	private:
		VkFramebuffer mFramebuffer;
        std::vector<FbAttachment> Attachments;
        std::vector<VkImageView> Views;
	public:

        VkFramebuffer GetFramebuffer() { return mFramebuffer; }

        void PushAttachment(unsigned int Width, unsigned int Height, VkFormat format, VkImageUsageFlagBits usage, VkImageAspectFlagBits aspect);
        void PushOutputAttachment();
        void PushDepthAttachment(unsigned int Width, unsigned int Height);

		bool Create(ZSVulkanPipeline* pipeline);
		ZSVulkanFramebuffer();
	};
}