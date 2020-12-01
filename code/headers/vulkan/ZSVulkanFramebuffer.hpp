#pragma once

#include "../render/Framebuffer.hpp"
#include <vulkan/vulkan.hpp>

namespace Engine {
	class ZSVulkanFramebuffer {
	private:
		VkFramebuffer mFramebuffer;
	public:
		ZSVulkanFramebuffer();
	};
}