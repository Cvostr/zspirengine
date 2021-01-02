#pragma once

#include <vulkan/vulkan.hpp>
#include "../render/GpuObject.hpp"

namespace Engine {
	class ZSVulkanSampler : public GpuObject {
	private:

		VkSampler mSampler;
	public:

		bool CreateSampler();
		VkSampler GetVkSampler();

		ZSVulkanSampler();
		~ZSVulkanSampler();
	};
}