#include "../../headers/vulkan/ZSVulkanPipelineLayout.hpp"
#include "../../headers/game.h"

extern ZSGAME_DATA* game_data;

using namespace Engine;

void ZsVkPipelineLayoutConf::AddPushConstant(unsigned int size, VkShaderStageFlagBits flag) {
    VkPushConstantRange PCRange = {};
    PCRange.offset = mPushConstantBuffersSize;
    PCRange.size = size;
    PCRange.stageFlags = flag;
    this->mPushConstants.push_back(PCRange);

    mPushConstantBuffersSize += size;
}

VkDescriptorSet* Engine::ZSVulkanPipelineLayout::GetDescriptorsSets() {
    return mDescrSets.data();
}
unsigned int Engine::ZSVulkanPipelineLayout::GetDescriptorSetsCount() {
    return static_cast<uint32_t>(mDescrSets.size());
}

void Engine::ZSVulkanPipelineLayout::CmdBindDescriptorSets(VkCommandBuffer cmdbuf) {
    vkCmdBindDescriptorSets(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, GetPipelineLayout(), 0, GetDescriptorSetsCount(), GetDescriptorsSets(), 0, nullptr);
}

void Engine::ZSVulkanPipelineLayout::BindTexture(_vk_Texture* Texture, ZSVulkanSampler* sampler, uint32_t slot) {
    DescrSetLayoutSampler->setTexture(slot, Texture->GetImageView(), sampler);
}

bool ZSVulkanPipelineLayout::Create(ZsVkPipelineLayoutConf& conf) {
    VkPipelineLayoutCreateInfo pipeline_info = {};
    pipeline_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    mDescrSetLayouts.push_back(conf.DescrSetLayout->getDescriptorSetLayout());
    mDescrSets.push_back(conf.DescrSetLayout->getDescriptorSet());

    mDescrSetLayouts.push_back(conf.DescrSetLayoutSampler->getDescriptorSetLayout());
    mDescrSets.push_back(conf.DescrSetLayoutSampler->getDescriptorSet());

    this->DescrSetLayoutSampler = conf.DescrSetLayoutSampler;

    pipeline_info.setLayoutCount = static_cast<uint32_t>(mDescrSetLayouts.size());
    pipeline_info.pSetLayouts = mDescrSetLayouts.data();
    pipeline_info.pushConstantRangeCount =
        static_cast<uint32_t>(conf.mPushConstants.size());
    pipeline_info.pPushConstantRanges = conf.mPushConstants.data();
    vkCreatePipelineLayout(game_data->vk_main->mDevice->getVkDevice(), &pipeline_info, nullptr, &this->mPipelineLayout);
    return true;
}