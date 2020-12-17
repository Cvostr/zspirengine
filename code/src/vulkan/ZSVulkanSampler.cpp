#include "../../headers/vulkan/ZSVulkanSampler.hpp"
#include "../../headers/game.h"

extern ZSGAME_DATA* game_data;

bool Engine::ZSVulkanSampler::CreateSampler() {
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;

	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	samplerInfo.anisotropyEnable = VK_FALSE;
	samplerInfo.maxAnisotropy = 16.0f;

	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	if (vkCreateSampler(game_data->vk_main->mDevice->getVkDevice(), &samplerInfo, nullptr, &this->sampler))
		throw std::runtime_error("VULKAN: Failed creating image sampler");
}
VkSampler Engine::ZSVulkanSampler::GetVkSampler() {
	return this->sampler;
}

Engine::ZSVulkanSampler::ZSVulkanSampler() {

}

Engine::ZSVulkanSampler::~ZSVulkanSampler() {

}