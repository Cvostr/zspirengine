#include <vulkan/VKRenderer.hpp>
#include <game.h>

using namespace Engine;
//Hack to support resources
extern ZSGAME_DATA* game_data;
extern ZSpireEngine* engine_ptr;

void VKMaterialTemplate::CreatePipeline() {
    if (engine_ptr->engine_info->graphicsApi == VULKAN && mShader->mCreated) {
        Engine::ZsVkPipelineConf Conf;
        MakeDescrSetUniform(Conf.LayoutInfo.DescrSetLayout);
        MakeDescrSetTextures(Conf.LayoutInfo.DescrSetLayoutSampler);
        MakeDescrSetStorage(Conf.LayoutInfo.DescrSetLayoutStorage);

        Conf.hasDepth = true;
        Conf.cullFace = true;
        Conf.LayoutInfo.AddPushConstant(64, VK_SHADER_STAGE_ALL_GRAPHICS);
        Conf.LayoutInfo.AddPushConstant(mUniformBuffer->GetBufferSize(), VK_SHADER_STAGE_FRAGMENT_BIT);

        Pipeline = new Engine::ZSVulkanPipeline;
        Pipeline->Create((Engine::vkShader*)mShader, game_data->vk_main->mGBufferRenderPass, Conf);
        mPipelineCreated = true;
    }
}

void VKMaterialTemplate::MakeDescrSetUniform(Engine::ZSVulkanDescriptorSet* DescrSet) {
    Engine::UniformBuffer* shadowBuf = game_data->pipeline->GetShadowmapUniformBuffer();
    
    VKRenderer* renderer = static_cast<VKRenderer*>(game_data->pipeline);

    DescrSet->pushStorageBuffer(renderer->GetTransformStorageBuffer(), VK_SHADER_STAGE_ALL_GRAPHICS);
    DescrSet->pushStorageBuffer(renderer->GetSkinningStorageBuffer(), VK_SHADER_STAGE_VERTEX_BIT);
    DescrSet->pushUniformBuffer((Engine::vkUniformBuffer*)shadowBuf, VK_SHADER_STAGE_FRAGMENT_BIT);
    DescrSet->getDescriptorSetLayout();
}

void VKMaterialTemplate::MakeDescrSetStorage(Engine::ZSVulkanDescriptorSet* DescrSet) {
    //Bind storage buffers
    VKRenderer* renderer = static_cast<VKRenderer*>(game_data->pipeline);

    DescrSet->pushStorageBuffer(renderer->GetCamerasStorageBuffer(), VK_SHADER_STAGE_ALL_GRAPHICS);
    DescrSet->getDescriptorSetLayout();
}

void VKMaterialTemplate::MakeDescrSetTextures(Engine::ZSVulkanDescriptorSet* DescrSet) {
    for (unsigned int prop_i = 0; prop_i < properties.size(); prop_i++) {
        MaterialShaderProperty* prop_ptr = properties[prop_i];
        if (prop_ptr->mType == MATSHPROP_TYPE_TEXTURE) {
            //Cast pointer
            TextureMaterialShaderProperty* texture_p = static_cast<TextureMaterialShaderProperty*>(prop_ptr);
            DescrSet->pushImageSampler(texture_p->slotToBind);
        }
    }
    DescrSet->getDescriptorSetLayout();
}


void VKMaterial::SetTexture(unsigned int slot, VkImageView view, Engine::ZSVulkanSampler* sampler) {
    this->DescrSetTextures->setTexture(slot, view, sampler);
}

void VKMaterial::CreateDescriptors() {
    if (engine_ptr->engine_info->graphicsApi == VULKAN && mTemplate->mShader->mCreated) {

        DescrSetTextures = new Engine::ZSVulkanDescriptorSet(Engine::DESCR_SET_TYPE::DESCR_SET_TYPE_TEXTURE);

        ((VKMaterialTemplate*)mTemplate)->MakeDescrSetTextures(DescrSetTextures);
    }
}