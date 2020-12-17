#pragma once

#include <vulkan/vulkan.hpp>

namespace Engine {
	class ZSVulkanSampler {
	private:

		

		VkSampler sampler;
	public:

		bool CreateSampler();
		VkSampler GetVkSampler();

		ZSVulkanSampler();
		~ZSVulkanSampler();
	};
}