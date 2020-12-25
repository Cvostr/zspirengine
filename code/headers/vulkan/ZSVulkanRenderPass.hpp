#pragma once

#include <vulkan/vulkan.hpp>

namespace Engine {
	class ZSVulkanFramebuffer;

	class ZSVulkanRenderPass {
	private:
		VkRenderPass mRenderPass;

		std::vector<VkAttachmentDescription> mAttachmentDescriptions;
		std::vector<VkAttachmentReference> mAttachmentReferences;
		VkAttachmentReference DepthDescriptionRef;
	public:
		VkRenderPass GetRenderPass() { return mRenderPass; }

		void PushColorAttachment(VkFormat Format, VkImageLayout Layout);
		void PushColorOutputAttachment();
		void PushDepthAttachment();

		bool Create();
		void CmdBegin(VkCommandBuffer cmdbuf, ZSVulkanFramebuffer* framebuffer);
	};
}