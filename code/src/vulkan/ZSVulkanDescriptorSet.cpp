#include "../../headers/vulkan/ZSVulkanDescriptorSet.hpp"
#include "../../headers/game.h"

extern ZSGAME_DATA* game_data;

void Engine::ZSVulkanDescriptorSet::pushUniformBuffer(vkUniformBuffer* buf, VkShaderStageFlags stageFlags) {

    if (type != DESCR_SET_TYPE::DESCR_SET_TYPE_UBO)
        return;

    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = buf->GetSlotId();
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = stageFlags;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    //Push description
    this->bindings.push_back(uboLayoutBinding);
    //Add pool for this buffer
    addDescriptorPool(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    ubuffers.push_back(buf);
}
void Engine::ZSVulkanDescriptorSet::pushStorageBuffer(vkUniformBuffer* buf, VkShaderStageFlags stageFlags) {
    if (type != DESCR_SET_TYPE::DESCR_SET_TYPE_UBO)
        return;

    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = buf->GetSlotId();
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = stageFlags;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    //Push description
    this->bindings.push_back(uboLayoutBinding);
    //Add pool for this buffer
    addDescriptorPool(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
    ubuffers.push_back(buf);
}

void Engine::ZSVulkanDescriptorSet::pushImageSampler(int slot) {

    if (type != DESCR_SET_TYPE::DESCR_SET_TYPE_TEXTURE)
        return;

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = slot;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    //Push description
    this->bindings.push_back(samplerLayoutBinding);
    //Add pool for this buffer
    addDescriptorPool(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
}
void Engine::ZSVulkanDescriptorSet::addDescriptorPool(VkDescriptorType type) {
    //Filling Size struct
    VkDescriptorPoolSize poolSize = getVkDescrPoolSize(type, 1);
    pool_sizes.push_back(poolSize);
}

VkDescriptorSetLayout Engine::ZSVulkanDescriptorSet::getDescriptorSetLayout() {

    VkDescriptorPoolCreateInfo poolInfo =
        getDescrPoolCreateInfo(static_cast<unsigned int>(pool_sizes.size()),
                                pool_sizes.data(), 1);

    //Create POOL
    if (vkCreateDescriptorPool(game_data->vk_main->mDevice->getVkDevice(), &poolInfo, nullptr, &this->Descr_Pool) != VK_SUCCESS) {
        throw std::runtime_error("VULKAN: DESCR POOL creation failed");
    }


    VkDescriptorSetLayoutCreateInfo cr_info = {};
    cr_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    cr_info.bindingCount = static_cast<uint32_t>(bindings.size());
    cr_info.pBindings = bindings.data();
    cr_info.pNext = nullptr;

    vkCreateDescriptorSetLayout(game_data->vk_main->mDevice->getVkDevice(), &cr_info, nullptr, &this->layout);

    VkDescriptorSetAllocateInfo allocInfo = getDescrSetAllocInfo(Descr_Pool, 1, &layout);
    vkAllocateDescriptorSets(game_data->vk_main->mDevice->getVkDevice(), &allocInfo, &Descr_Set);

    if (type == DESCR_SET_TYPE::DESCR_SET_TYPE_UBO) {
        for (int i = 0; i < bindings.size(); i++) {
            UpdUniformBuffer(i);
        }
    }

    return layout;
}

VkDescriptorSet Engine::ZSVulkanDescriptorSet::getDescriptorSet() {
    return Descr_Set;
}

void Engine::ZSVulkanDescriptorSet::UpdUniformBuffer(unsigned int index) {
    if (type == DESCR_SET_TYPE::DESCR_SET_TYPE_UBO) {
        vkUniformBuffer* buf = ubuffers[index];
        //Generating buffer
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.offset = 0;
        bufferInfo.range = buf->GetBufferSize();
        bufferInfo.buffer = buf->GetBuffer();

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = Descr_Set;
        descriptorWrite.dstBinding = buf->GetSlotId();
        descriptorWrite.dstArrayElement = 0;

        VkDescriptorType DescrType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        if (buf->isStorageBuffer())
            DescrType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

        descriptorWrite.descriptorType = DescrType;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = nullptr; // Optional
        descriptorWrite.pTexelBufferView = nullptr; // Optional
        vkUpdateDescriptorSets(game_data->vk_main->mDevice->getVkDevice(), 1, &descriptorWrite, 0, nullptr);
    }
}

void Engine::ZSVulkanDescriptorSet::setTexture(int index, VkImageView image, ZSVulkanSampler* sampler) {
    if (type != DESCR_SET_TYPE::DESCR_SET_TYPE_TEXTURE)
        return;

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = image;
    imageInfo.sampler = sampler->GetVkSampler();


    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = Descr_Set;
    descriptorWrite.dstBinding = index;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = nullptr;
    descriptorWrite.pImageInfo = &imageInfo; // Optional
    descriptorWrite.pTexelBufferView = nullptr; // Optional
    vkUpdateDescriptorSets(game_data->vk_main->mDevice->getVkDevice(), 1, &descriptorWrite, 0, nullptr);
}

Engine::ZSVulkanDescriptorSet::ZSVulkanDescriptorSet(DESCR_SET_TYPE type) {
    this->type = type;
}

//Some Vulkan constructors
VkDescriptorPoolSize Engine::getVkDescrPoolSize(VkDescriptorType type, unsigned int descr_count) {
    VkDescriptorPoolSize poolSize{};
    poolSize.type = type;
    poolSize.descriptorCount = descr_count;
    return poolSize;
}
VkDescriptorPoolCreateInfo Engine::getDescrPoolCreateInfo(unsigned int pools, VkDescriptorPoolSize* sizes, unsigned int maxSets) {
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = pools;
    poolInfo.pPoolSizes = sizes;
    poolInfo.maxSets = maxSets;

    return poolInfo;
}
VkDescriptorSetAllocateInfo Engine::getDescrSetAllocInfo(VkDescriptorPool pool, unsigned int Descr_Set_Count, VkDescriptorSetLayout* layout) {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = pool;
    allocInfo.descriptorSetCount = Descr_Set_Count;
    allocInfo.pSetLayouts = layout;

    return allocInfo;
}