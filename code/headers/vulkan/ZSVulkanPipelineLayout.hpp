#pragma once

#include <vulkan/vulkan.hpp>
#include "ZSVulkanDescriptorSet.hpp"
#include "VKTexture.hpp"
#include "ZSVulkanSampler.hpp"

namespace Engine {

	struct ZsVkPipelineLayoutConf {
		ZSVulkanDescriptorSet* DescrSetLayout;
		ZSVulkanDescriptorSet* DescrSetLayoutSampler;

		unsigned int mPushConstantBuffersSize;
		std::vector<VkPushConstantRange> mPushConstants;

		void AddPushConstant(unsigned int size, VkShaderStageFlagBits flag);

		ZsVkPipelineLayoutConf() {
			mPushConstantBuffersSize = 0;
			DescrSetLayout = new ZSVulkanDescriptorSet(DESCR_SET_TYPE::DESCR_SET_TYPE_UBO);
			DescrSetLayoutSampler = new ZSVulkanDescriptorSet(DESCR_SET_TYPE::DESCR_SET_TYPE_TEXTURE);
		}
	};

	class ZSVulkanPipelineLayout {
	private:
		VkPipelineLayout mPipelineLayout;

		std::vector<VkDescriptorSetLayout> mDescrSetLayouts; //Array of all layouts
		std::vector<VkDescriptorSet> mDescrSets; //Array of all descriptors

		ZSVulkanDescriptorSet* DescrSetLayoutSampler;
	public:

		VkDescriptorSet* GetDescriptorsSets();
		unsigned int GetDescriptorSetsCount();

		void CmdBindDescriptorSets(VkCommandBuffer cmdbuf);
		void BindTexture(vkTexture* Texture, ZSVulkanSampler* sampler, uint32_t slot);

		VkPipelineLayout GetPipelineLayout() {
			return mPipelineLayout;
		}
		bool Create(ZsVkPipelineLayoutConf& conf);
	};
}