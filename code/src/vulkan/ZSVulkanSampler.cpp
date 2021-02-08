#include "../../headers/vulkan/ZSVulkanSampler.hpp"
#include "../../headers/game.h"

extern ZSGAME_DATA* game_data;

VkFilter GetSamplerFilterGL(Engine::TextureFilteringMode TFM) {
	VkFilter result = VK_FILTER_NEAREST;

	if (TFM == Engine::SAMPLER_FILTERING_LINEAR)
		result = VK_FILTER_LINEAR;
	if (TFM == Engine::SAMPLER_FILTERING_NEAREST)
		result = VK_FILTER_NEAREST;

	return result;
}

VkSamplerAddressMode GetSamplerWrapMode(Engine::TextureWrapMode TWM) {
	VkSamplerAddressMode result = VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;

	if (TWM == Engine::SAMPLER_WRAP_REPEAT)
		result = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	if (TWM == Engine::SAMPLER_WRAP_MIRRORED_REPEAT)
		result = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	if (TWM == Engine::SAMPLER_WRAP_CLAMP_TO_EDGE)
		result = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

	return result;
}

void Engine::ZSVulkanSampler::CreateSampler() {
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = GetSamplerFilterGL(mMagFiltering);
	samplerInfo.minFilter = GetSamplerFilterGL(mMinFiltering);

	samplerInfo.addressModeU = GetSamplerWrapMode(mWrapU);
	samplerInfo.addressModeV = GetSamplerWrapMode(mWrapV);
	samplerInfo.addressModeW = GetSamplerWrapMode(mWrapW);

	samplerInfo.anisotropyEnable = (mMaxAnisotropy > 1.f);
	samplerInfo.maxAnisotropy = mMaxAnisotropy;

	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	if (vkCreateSampler(game_data->vk_main->mDevice->getVkDevice(), &samplerInfo, nullptr, &this->mSampler)) {
		throw std::runtime_error("VULKAN: Failed creating image sampler");
	}

	mCreated = true;

}
VkSampler Engine::ZSVulkanSampler::GetVkSampler() {
	return this->mSampler;
}

void Engine::ZSVulkanSampler::Destroy() {
	if (mCreated) {
		vkDestroySampler(game_data->vk_main->mDevice->getVkDevice(), mSampler, nullptr);
		mCreated = false;
	}
}

Engine::ZSVulkanSampler::~ZSVulkanSampler() {
	Destroy();
}