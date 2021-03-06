#pragma once

#include "../engine/EngineComponent.h"

#include "ZSVulkanInstance.hpp"
#include "ZSVulkanDevice.hpp"
#include "ZSVulkanSwapchain.hpp"
#include "ZSVulkanRenderPass.hpp"
#include "ZSVulkanSampler.hpp"
#include "ZSVMA.hpp"

namespace Engine {
	class ZSVulkan : public IEngineComponent {
	public:
		ZSVulkanInstance* mInstance;
		ZSVulkanDevice* mDevice;
		ZSVulkanSwapChain* mSwapChain;
		ZSVMA* mVMA;

		VkCommandBuffer CurrentCmdBuffer;

		ZSVulkanRenderPass* mGBufferRenderPass;
		ZSVulkanRenderPass* mDefferedRenderPass;

		ZSVulkanSampler* mDefaultTextureSampler;

		ZSVulkan() 
		{

		}
	};
}