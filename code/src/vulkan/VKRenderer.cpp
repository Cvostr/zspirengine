#include "../../headers/vulkan/VKRenderer.hpp"
#include "../../headers/game.h"
#include "../../headers/world/ObjectsComponents/MeshComponent.hpp"
#include "../../headers/world/ObjectsComponents/MaterialComponent.hpp"
#include "../../headers/world/ObjectsComponents/ShadowCasterComponent.hpp"

extern ZSGAME_DATA* game_data;
extern ZSpireEngine* engine_ptr;

Engine::VKRenderer::VKRenderer() {
	InitShaders();
}

void Engine::VKRenderer::render2D() {

}
void Engine::VKRenderer::render3D(Engine::Camera* cam) {
    World* world_ptr = game_data->world;
    ObjectsToRender.clear();
    LastTransformOffset = 0;
    LastSkinningOffset = 0;
    //Fill render arrays

    if (this->render_settings.shadowcaster_obj_ptr != nullptr) {
        Engine::ShadowCasterProperty* shadowcast =
            static_cast<Engine::GameObject*>(this->render_settings.shadowcaster_obj_ptr)->getPropertyPtr<Engine::ShadowCasterProperty>();
        if (shadowcast != nullptr) { //we have shadowcaster
            shadowcast->SendShadowParamsToShaders(cam, this); //draw shadowcaster
        }
    }

    processObjects(world_ptr);
    FillShadowCmdBuf();
    Fill3dCmdBuf();
    
 

    Present();
}

void Engine::VKRenderer::DrawObject(Engine::GameObject* obj) {



    if (current_state == PIPELINE_STATE::PIPELINE_STATE_DEFAULT)
        //Call prerender on each property in object
        obj->onPreRender(this);
    if (obj->hasMesh() //|| obj->hasTerrain()
        ) {
        VKObjectToRender obr;
        //Send transform matrix to transform buffer
        Engine::TransformProperty* transform_ptr = obj->getTransformProperty();
        Engine::MeshProperty* mesh_prop = obj->getPropertyPtr<Engine::MeshProperty>();
        Engine::MaterialProperty* mat_prop = obj->getPropertyPtr<Engine::MaterialProperty>();

        
       
        if (mesh_prop->mesh_ptr->resource_state != RESOURCE_STATE::STATE_LOADED)
            mesh_prop->mesh_ptr->load();

        TransformStorageBuf->writeDataBuffered(LastTransformOffset * sizeof(Mat4), sizeof(Mat4), &transform_ptr->transform_mat);
        obr.TransformArrayIndex = LastTransformOffset;
        LastTransformOffset++;

        unsigned int BonesCount = obj->getBonesCount();
        if (BonesCount) {
            obj->setSkinningMatrices(this);
            //Send bones
            SkinningStorageBuf->writeDataBuffered(LastSkinningOffset * sizeof(Mat4), BonesCount * sizeof(Mat4), skinningUniformBuffer->GetCpuBuffer());
            obr.SkinningArrayIndex = LastSkinningOffset;
            LastSkinningOffset += BonesCount;
        }

        obr.transform = transform_ptr->transform_mat;
        
        obr.obj = obj;

        if (mat_prop != nullptr) {
            obr.mat = (VKMaterial*)mat_prop->mMaterial;
            if(obr.mat != nullptr)
                obr.mat->applyMatToPipeline();
        }

        this->ObjectsToRender.push_back(obr);
    }
}

void Engine::VKRenderer::InitShaders() {
    this->default3d->compileFromFile("Shaders/vulkan_test/3d/vert.spv", "Shaders/vulkan_test/3d/frag.spv");
    this->deffered_light->compileFromFile("Shaders/vulkan_test/deffered/vert.spv", "Shaders/vulkan_test/deffered/frag.spv");
    this->mShadowMapShader->compileFromFile("Shaders/vulkan_test/shadowmap/vert.spv", "Shaders/vulkan_test/shadowmap/frag.spv", "Shaders/vulkan_test/shadowmap/geom.spv");

    MaterialRenderPass = new ZSVulkanRenderPass;
    MaterialRenderPass->PushColorAttachment(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    MaterialRenderPass->PushColorAttachment(VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    MaterialRenderPass->PushColorAttachment(VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    MaterialRenderPass->PushDepthAttachment();
    MaterialRenderPass->Create();

    game_data->vk_main->mMaterialsRenderPass = MaterialRenderPass;

    mMaterialSampler = new ZSVulkanSampler;
    mMaterialSampler->CreateSampler();
    game_data->vk_main->mDefaultTextureSampler = mMaterialSampler;


    MaterialFb = new ZSVulkanFramebuffer;
    MaterialFb->PushAttachment(640, 480, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
    MaterialFb->PushAttachment(640, 480, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
    MaterialFb->PushAttachment(640, 480, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
    MaterialFb->PushDepthAttachment(640, 480);
    MaterialFb->Create(MaterialRenderPass);
    
    TransformStorageBuf = static_cast<vkUniformBuffer*>(allocUniformBuffer());
    TransformStorageBuf->init(0, sizeof(Mat4) * 4000, true);

    SkinningStorageBuf = static_cast<vkUniformBuffer*>(allocUniformBuffer());
    SkinningStorageBuf->init(1, sizeof(Mat4) * 200 * 512, true);

    OutRenderPass = new ZSVulkanRenderPass;
    OutRenderPass->PushColorOutputAttachment();
    OutRenderPass->Create();

    OutFb = new ZSVulkanFramebuffer;
    OutFb->PushOutputAttachment();
    OutFb->Create(OutRenderPass);

    ShadowRenderPass = new ZSVulkanRenderPass;
    ShadowRenderPass->PushDepthAttachment();
    ShadowRenderPass->Create();
    ShadowRenderPass->SetClearSize(4096, 4096);
    

    ShadowFb = new ZSVulkanFramebuffer;
    ShadowFb->PushDepthAttachment(4096, 4096, 2);
    ShadowFb->SetLayersCount(2);
    ShadowFb->Create(ShadowRenderPass);


    Engine::ZsVkPipelineConf Conf;
    Conf.hasDepth = false;
    Conf.cullFace = false;
    Conf.LayoutInfo.DescrSetLayout->pushUniformBuffer((Engine::vkUniformBuffer*)this->transformBuffer, VK_SHADER_STAGE_FRAGMENT_BIT);
    Conf.LayoutInfo.DescrSetLayout->pushUniformBuffer((Engine::vkUniformBuffer*)this->lightsBuffer, VK_SHADER_STAGE_FRAGMENT_BIT);

    Conf.LayoutInfo.DescrSetLayoutSampler->pushImageSampler(0);
    Conf.LayoutInfo.DescrSetLayoutSampler->pushImageSampler(1);
    Conf.LayoutInfo.DescrSetLayoutSampler->pushImageSampler(2);
   
    DefferedPipeline = new Engine::ZSVulkanPipeline;
    DefferedPipeline->Create((Engine::vkShader*)deffered_light, OutRenderPass, Conf);
    Conf.LayoutInfo.DescrSetLayoutSampler->setTexture(0, MaterialFb->getImageViewIndex(0), mMaterialSampler);
    Conf.LayoutInfo.DescrSetLayoutSampler->setTexture(1, MaterialFb->getImageViewIndex(1), mMaterialSampler);
    Conf.LayoutInfo.DescrSetLayoutSampler->setTexture(2, MaterialFb->getImageViewIndex(2), mMaterialSampler);
    



    Engine::ZsVkPipelineConf ConfShadow;
    ConfShadow.hasDepth = true;
    ConfShadow.cullFace = false;
    ConfShadow.LayoutInfo.DescrSetLayout->pushUniformBuffer((Engine::vkUniformBuffer*)this->transformBuffer, VK_SHADER_STAGE_VERTEX_BIT);
    ConfShadow.LayoutInfo.DescrSetLayout->pushUniformBuffer((Engine::vkUniformBuffer*)this->shadowBuffer, VK_SHADER_STAGE_VERTEX_BIT);
    ConfShadow.LayoutInfo.DescrSetLayoutStorage->pushStorageBuffer(GetTransformStorageBuffer(), VK_SHADER_STAGE_VERTEX_BIT);
    ConfShadow.LayoutInfo.DescrSetLayoutStorage->pushStorageBuffer(GetSkinningStorageBuffer(), VK_SHADER_STAGE_VERTEX_BIT);

    ConfShadow.LayoutInfo.AddPushConstant(64, VK_SHADER_STAGE_VERTEX_BIT);
    ConfShadow.Viewport.width = 4096;
    ConfShadow.Viewport.height = 4096;

    ShadowPipeline = new Engine::ZSVulkanPipeline;
    ShadowPipeline->Create((Engine::vkShader*)mShadowMapShader, ShadowRenderPass, ConfShadow);



	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	vkCreateSemaphore(game_data->vk_main->mDevice->getVkDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphore);
	vkCreateSemaphore(game_data->vk_main->mDevice->getVkDevice(), &semaphoreInfo, nullptr, &MaterialsFinishedSemaphore);
    vkCreateSemaphore(game_data->vk_main->mDevice->getVkDevice(), &semaphoreInfo, nullptr, &DefferedFinishedSemaphore);
    vkCreateSemaphore(game_data->vk_main->mDevice->getVkDevice(), &semaphoreInfo, nullptr, &ShadowFinishedSemaphore);
    

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = game_data->vk_main->mDevice->GetGraphicsQueueFamilyIndex();
    poolInfo.flags = 0; // Optional

    vkCreateCommandPool(game_data->vk_main->mDevice->getVkDevice(), &poolInfo, nullptr, &commandPool);

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)1;

    vkAllocateCommandBuffers(game_data->vk_main->mDevice->getVkDevice(), &allocInfo, &m3dCmdBuf);
    vkAllocateCommandBuffers(game_data->vk_main->mDevice->getVkDevice(), &allocInfo, &mDefferedCmdBuf);
    vkAllocateCommandBuffers(game_data->vk_main->mDevice->getVkDevice(), &allocInfo, &mShadowCmdBuf);

    FillDefferedCmdBuf();

    if (engine_ptr->desc->game_perspective == PERSP_3D) {
        MtShProps::genDefaultMtShGroup(default3d, mSkyboxShader, mTerrainShader, water_shader);
    }
}

void Engine::VKRenderer::FillShadowCmdBuf() {
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    vkBeginCommandBuffer(mShadowCmdBuf, &beginInfo);

    game_data->vk_main->CurrentCmdBuffer = mShadowCmdBuf;

    ShadowRenderPass->CmdBegin(mShadowCmdBuf, ShadowFb);

    ShadowPipeline->CmdBindPipeline(mShadowCmdBuf);
    ShadowPipeline->CmdBindDescriptorSets(mShadowCmdBuf);
    //SetViewport(mShadowCmdBuf, 0, 0, 4096, 4096);

    for (unsigned int i = 0; i < ObjectsToRender.size(); i++) {
        VKObjectToRender* obr = &ObjectsToRender[i];
        GameObject* obj = ObjectsToRender[i].obj;
        if (obj->hasMesh() && obr->mat != nullptr) {
            MeshProperty* mesh = obj->getPropertyPtr<MeshProperty>();
       
            //Send object transform
            ShadowPipeline->CmdPushConstants(mShadowCmdBuf, VK_SHADER_STAGE_VERTEX_BIT, 0, 4, &obr->TransformArrayIndex);
            ShadowPipeline->CmdPushConstants(mShadowCmdBuf, VK_SHADER_STAGE_VERTEX_BIT, 4, 4, &obr->SkinningArrayIndex);

            if (mesh->mesh_ptr->resource_state == RESOURCE_STATE::STATE_LOADED)
                obj->DrawMeshInstanced(this, 2);
            
        }
    }


    vkCmdEndRenderPass(mShadowCmdBuf);
    vkEndCommandBuffer(mShadowCmdBuf);
}

void Engine::VKRenderer::Fill3dCmdBuf() {
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    vkBeginCommandBuffer(m3dCmdBuf, &beginInfo);

    game_data->vk_main->CurrentCmdBuffer = m3dCmdBuf;

    MaterialRenderPass->CmdBegin(m3dCmdBuf, MaterialFb);

    bool binded = false;

    for (unsigned int i = 0; i < ObjectsToRender.size(); i++) {
        VKObjectToRender* obr = &ObjectsToRender[i];
        GameObject* obj = ObjectsToRender[i].obj;
        if (obj->hasMesh() && obr->mat != nullptr) {
            MeshProperty* mesh = obj->getPropertyPtr<MeshProperty>();
            ZSVulkanPipeline* Pipeline = ((VKMaterialTemplate*)obr->mat->mTemplate)->Pipeline;
            if (((VKMaterialTemplate*)obr->mat->mTemplate)->mPipelineCreated) {
                if (!binded) {
                    Pipeline->CmdBindPipeline(m3dCmdBuf);

                    ((VKMaterialTemplate*)obr->mat->mTemplate)->Pipeline->GetPipelineLayout()
                        ->CmdBindDescriptorSets(m3dCmdBuf, 0, 3);

                    binded = true;
                }


                VkDescriptorSet set = obr->mat->DescrSetTextures->getDescriptorSet();
                if (Pipeline != nullptr) {

                    vkCmdBindDescriptorSets(m3dCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS,
                        Pipeline->_GetPipelineLayout(), 1,
                        1, &set, 0, nullptr);
                    //Send object transform
                    Pipeline->CmdPushConstants(m3dCmdBuf, VK_SHADER_STAGE_VERTEX_BIT, 0, 4, &obr->TransformArrayIndex);
                    Pipeline->CmdPushConstants(m3dCmdBuf, VK_SHADER_STAGE_VERTEX_BIT, 4, 4, &obr->SkinningArrayIndex);
                    //Send material props
                    unsigned int bufsize = obr->mat->mTemplate->mUniformBuffer->GetBufferSize();
                    void* bufdata = obr->mat->mTemplate->mUniformBuffer->GetCpuBuffer();
                    Pipeline->CmdPushConstants(m3dCmdBuf, VK_SHADER_STAGE_FRAGMENT_BIT, 64, bufsize, obr->mat->MatData);
                    
                }
                if (mesh->mesh_ptr->resource_state == RESOURCE_STATE::STATE_LOADED)
                    obj->DrawMesh(this);
            }
        }
    }


    vkCmdEndRenderPass(m3dCmdBuf);
    vkEndCommandBuffer(m3dCmdBuf);
}

void Engine::VKRenderer::FillDefferedCmdBuf() {
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    vkBeginCommandBuffer(mDefferedCmdBuf, &beginInfo);

    game_data->vk_main->CurrentCmdBuffer = mDefferedCmdBuf;

    OutRenderPass->CmdBegin(mDefferedCmdBuf, OutFb);

    DefferedPipeline->CmdBindPipeline(mDefferedCmdBuf);
    DefferedPipeline->CmdBindDescriptorSets(mDefferedCmdBuf);

    Engine::getPlaneMesh2D()->Draw();

    vkCmdEndRenderPass(mDefferedCmdBuf);
    vkEndCommandBuffer(mDefferedCmdBuf);
}

void Engine::VKRenderer::Present() {
    uint32_t _imageIndex;
    vkAcquireNextImageKHR(game_data->vk_main->mDevice->getVkDevice(), game_data->vk_main->mSwapChain->GetSwapChain(), UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &_imageIndex);
    _imageIndex = 0;


    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &imageAvailableSemaphore;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m3dCmdBuf;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &MaterialsFinishedSemaphore;
    vkQueueSubmit(game_data->vk_main->mDevice->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);


    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &MaterialsFinishedSemaphore;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &mShadowCmdBuf;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &ShadowFinishedSemaphore;
    vkQueueSubmit(game_data->vk_main->mDevice->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    

    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &ShadowFinishedSemaphore;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &mDefferedCmdBuf;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &DefferedFinishedSemaphore;
    vkQueueSubmit(game_data->vk_main->mDevice->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);


    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &DefferedFinishedSemaphore;

    VkSwapchainKHR swapChains[] = { game_data->vk_main->mSwapChain->GetSwapChain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    uint32_t imageIndex = 0;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional
    vkQueuePresentKHR(game_data->vk_main->mDevice->GetPresentQueue(), &presentInfo);
}

void Engine::VKRenderer::SetViewport(VkCommandBuffer cmdbuf, unsigned int startX, unsigned int startY, unsigned int width, unsigned int height) {
    VkViewport viewport = {startX, startY, width, height, 0, 1};
    vkCmdSetViewport(cmdbuf, 0, 1, &viewport);
}