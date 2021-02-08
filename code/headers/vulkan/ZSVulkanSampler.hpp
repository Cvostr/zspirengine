#pragma once

#include <vulkan/vulkan.hpp>
#include "../render/TextureSampler.hpp"

namespace Engine {
	class ZSVulkanSampler : public TextureSampler {
	private:
		VkSampler mSampler;
	public:

		void CreateSampler();
		void Destroy();

		VkSampler GetVkSampler();

		ZSVulkanSampler() {
			TextureSampler();
		}

		~ZSVulkanSampler();
	};
}