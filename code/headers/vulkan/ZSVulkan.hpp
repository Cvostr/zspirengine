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

		ZSVulkanRenderPass* mMaterialsRenderPass;
		ZSVulkanSampler* mDefaultTextureSampler;
		bool MaterialsBindTexturesStage;

		ZSVulkan() :
			MaterialsBindTexturesStage(false)
		{

		}
	};
}