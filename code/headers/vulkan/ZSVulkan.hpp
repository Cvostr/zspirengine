#pragma once

#include "../engine/EngineComponent.h"

#include "ZSVulkanInstance.hpp"
#include "ZSVulkanDevice.hpp"
#include "ZSVulkanSwapchain.hpp"
#include "ZSVMA.hpp"

namespace Engine {
	class ZSVulkan : public IEngineComponent {
	public:
		ZSVulkanInstance* mInstance;
		ZSVulkanDevice* mDevice;
		ZSVulkanSwapChain* mSwapChain;
		ZSVMA* mVMA;

		VkCommandBuffer CurrentCmdBuffer;
	};
}