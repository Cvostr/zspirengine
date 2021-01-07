#include "../../headers/vulkan/VKRenderer.hpp"
#include "../../headers/game.h"
#include "../../headers/world/ObjectsComponents/MeshComponent.hpp"
#include "../../headers/world/ObjectsComponents/MaterialComponent.hpp"

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
    //Fill render arrays
    processObjects(world_ptr);

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

        obr.transform = transform_ptr->transform_mat;
        obr.obj = obj;

        if (mat_prop != nullptr) {
            obr.mat = mat_prop->mMaterial;
            if(obr.mat != nullptr)
                obr.mat->applyMatToPipeline();
        }

        this->ObjectsToRender.push_back(obr);
    }
}

void Engine::VKRenderer::InitShaders() {
    this->default3d->compileFromFile("Shaders/vulkan_test/3d/vert.spv", "Shaders/vulkan_test/3d/frag.spv");
    this->deffered_light->compileFromFile("Shaders/vulkan_test/deffered/vert.spv", "Shaders/vulkan_test/deffered/frag.spv");


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
    

    OutRenderPass = new ZSVulkanRenderPass;
    OutRenderPass->PushColorOutputAttachment();
    OutRenderPass->Create();

    OutFb = new ZSVulkanFramebuffer;
    OutFb->PushOutputAttachment();
    OutFb->Create(OutRenderPass);



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
    


	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	vkCreateSemaphore(game_data->vk_main->mDevice->getVkDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphore);
	vkCreateSemaphore(game_data->vk_main->mDevice->getVkDevice(), &semaphoreInfo, nullptr, &MaterialsFinishedSemaphore);
    vkCreateSemaphore(game_data->vk_main->mDevice->getVkDevice(), &semaphoreInfo, nullptr, &DefferedFinishedSemaphore);

    

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

    FillDefferedCmdBuf();

    if (engine_ptr->desc->game_perspective == PERSP_3D) {
        MtShProps::genDefaultMtShGroup(default3d, mSkyboxShader, mTerrainShader, water_shader);
    }
}

void Engine::VKRenderer::Fill3dCmdBuf() {
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    vkBeginCommandBuffer(m3dCmdBuf, &beginInfo);

    game_data->vk_main->CurrentCmdBuffer = m3dCmdBuf;

    MaterialRenderPass->CmdBegin(m3dCmdBuf, MaterialFb);

    //game_data->vk_main->MaterialsBindTexturesStage = false;

    bool binded = false;

    for (unsigned int i = 0; i < ObjectsToRender.size(); i++) {
        VKObjectToRender* obr = &ObjectsToRender[i];
        GameObject* obj = ObjectsToRender[i].obj;
        if (obj->hasMesh() && obr->mat != nullptr) {
            MeshProperty* mesh = obj->getPropertyPtr<MeshProperty>();
            if (!binded) {
                obr->mat->mTemplate->Pipeline->CmdBindPipeline(m3dCmdBuf);
                binded = true;
            }

            VkDescriptorSet sets[2];
            sets[0] = obr->mat->DescrSetUBO->getDescriptorSet();
            sets[1] = obr->mat->DescrSetTextures->getDescriptorSet();
            if (obr->mat->mTemplate->Pipeline != nullptr) {
                
                obr->mat->applyMatToPipeline();
                vkCmdBindDescriptorSets(m3dCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    obr->mat->mTemplate->Pipeline->_GetPipelineLayout(), 0,
                    2, sets, 0, nullptr);
                //Send object transform
                obr->mat->mTemplate->Pipeline->CmdPushConstants(this->m3dCmdBuf, VK_SHADER_STAGE_VERTEX_BIT, 0, 64, &ObjectsToRender[i].transform);
                //Send material props
                unsigned int bufsize = obr->mat->mTemplate->mUniformBuffer->GetBufferSize();
                void* bufdata = obr->mat->mTemplate->mUniformBuffer->GetCpuBuffer();
                obr->mat->mTemplate->Pipeline->CmdPushConstants(m3dCmdBuf, VK_SHADER_STAGE_FRAGMENT_BIT, 64, bufsize, bufdata);
            }
            if (mesh->mesh_ptr->resource_state == RESOURCE_STATE::STATE_LOADED)
                obj->DrawMesh(this);
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