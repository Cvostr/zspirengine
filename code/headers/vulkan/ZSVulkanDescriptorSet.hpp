#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>
#include "VKUniformBuffer.hpp"
#include "ZSVulkanSampler.hpp"

namespace Engine {

	enum class DESCR_SET_TYPE {
		DESCR_SET_TYPE_UBO = 1,
		DESCR_SET_TYPE_TEXTURE = 2
	};

	class ZSVulkanDescriptorSet {
	private:
		DESCR_SET_TYPE type;

		VkDescriptorSetLayout layout;
		std::vector<VkDescriptorSetLayoutBinding> bindings;
		std::vector<VkDescriptorPoolSize> pool_sizes;
		VkDescriptorPool Descr_Pool; //Descriptor Pool
		VkDescriptorSet Descr_Set; //Descriptor Set
		//Array of pointers to Uniform Objects
		std::vector<vkUniformBuffer*> ubuffers;

		void addDescriptorPool(VkDescriptorType type);

	public:
		void UpdUniformBuffer(unsigned int index);
		void pushUniformBuffer(vkUniformBuffer* buf, VkShaderStageFlags stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS);
		void pushStorageBuffer(vkUniformBuffer* buf, VkShaderStageFlags stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS);
		void pushImageSampler(int slot);
		void pushImageSamplers(int FirstSlot, int LastSlot);

		void setTexture(int index, VkImageView image, ZSVulkanSampler* sampler);

		VkDescriptorSetLayout getDescriptorSetLayout();
		VkDescriptorSet getDescriptorSet();

		ZSVulkanDescriptorSet(DESCR_SET_TYPE type);
	};

	VkDescriptorPoolSize getVkDescrPoolSize(VkDescriptorType type, unsigned int descr_count);
	VkDescriptorPoolCreateInfo getDescrPoolCreateInfo(unsigned int pools, VkDescriptorPoolSize* sizes, unsigned int maxSets);
	VkDescriptorSetAllocateInfo getDescrSetAllocInfo(VkDescriptorPool pool, unsigned int Descr_Set_Count, VkDescriptorSetLayout* layout);
}