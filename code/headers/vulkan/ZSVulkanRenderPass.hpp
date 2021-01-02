#pragma once

#include <vulkan/vulkan.hpp>
#include "../render/GpuObject.hpp"

namespace Engine {
	class ZSVulkanFramebuffer;

	class ZSVulkanRenderPass : public GpuObject{
	private:
		VkRenderPass mRenderPass;
		bool mHasDepthAttachment;
		std::vector<VkAttachmentDescription> mAttachmentDescriptions;
		std::vector<VkAttachmentReference> mAttachmentReferences;
		VkAttachmentReference DepthDescriptionRef;
		VkClearValue* mClearValues;
		int mClearValuesCount;
	public:
		VkRenderPass GetRenderPass() { return mRenderPass; }

		void PushColorAttachment(VkFormat Format, VkImageLayout Layout);
		void PushColorOutputAttachment();
		void PushDepthAttachment();

		bool Create();
		void CmdBegin(VkCommandBuffer cmdbuf, ZSVulkanFramebuffer* framebuffer);

		ZSVulkanRenderPass();
		~ZSVulkanRenderPass();
	};
}