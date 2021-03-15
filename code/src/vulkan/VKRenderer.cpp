#include <vulkan/VKRenderer.hpp>
#include <game.h>
#include <world/ObjectsComponents/MeshComponent.hpp>
#include <world/ObjectsComponents/MaterialComponent.hpp>
#include <world/ObjectsComponents/ShadowCasterComponent.hpp>
#include <world/ObjectsComponents/CameraComponent.hpp>

extern ZSGAME_DATA* game_data;
extern ZSpireEngine* engine_ptr;

Engine::VKRenderer::VKRenderer() {
	InitShaders();
}

void Engine::VKRenderer::render2D() {

}
void Engine::VKRenderer::render3D() {
    World* world_ptr = game_data->world;
    Engine::Window* win = engine_ptr->GetWindow();
    ObjectsToRender.clear();
    LastTransformOffset = 0;
    LastSkinningOffset = 0;
    //Fill render arrays

    if (mCameras.size() != CamerasToRender.size()) {
        CamerasToRender.resize(mCameras.size());

        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)1;

        for (unsigned int cam_i = 0; cam_i < mCameras.size(); cam_i++) {


            vkAllocateCommandBuffers(game_data->vk_main->mDevice->getVkDevice(), &allocInfo, &CamerasToRender[cam_i].DefferedCmdBuf);
            vkAllocateCommandBuffers(game_data->vk_main->mDevice->getVkDevice(), &allocInfo, &CamerasToRender[cam_i].GBufferCmdBuf);
        }
    }

    processObjects(world_ptr);

    for (unsigned int cam_i = 0; cam_i < mCameras.size(); cam_i++) {
        CamerasToRender[cam_i].camera_prop = mCameras[cam_i];
        CamerasToRender[cam_i].Index = cam_i;

        Render3DCamera(mCameras[cam_i], cam_i);
    }


   /* if (this->render_settings.shadowcaster_obj_ptr != nullptr) {
        Engine::ShadowCasterProperty* shadowcast =
            static_cast<Engine::GameObject*>(this->render_settings.shadowcaster_obj_ptr)->getPropertyPtr<Engine::ShadowCasterProperty>();
        if (shadowcast != nullptr) { //we have shadowcaster
            shadowcast->SendShadowParamsToShaders(cam, this); //draw shadowcaster
        }
    }

    FillShadowCmdBuf();
    
 */
    FillFinalCmdBufs();

    ComputeAll();
}

void Engine::VKRenderer::Render3DCamera(void* cam_prop, uint32_t cam_index) {
        CameraComponent* cc = (CameraComponent*)(cam_prop);
        VKCameraToRender* CamRender = &CamerasToRender[cam_index];
        World* world_ptr = game_data->world;

        VkCommandBuffer gbuffer_cmdbuf = CamRender->GBufferCmdBuf;
        VkCommandBuffer deffered_cmdbuf = CamRender->DefferedCmdBuf;

        ZSVulkanFramebuffer* gbuffer_fb = static_cast<ZSVulkanFramebuffer*>(cc->mGBuffer);
        ZSVulkanFramebuffer* deffered_fb = static_cast<ZSVulkanFramebuffer*>(cc->mDefferedBuffer);

        //Check, is camera active
        if (!cc->isActive())
            return;

        Camera* _cam = (Camera*)cc;

        if (!cc->mIsMainCamera) {
            cc->UpdateTextureResource();
            Texture* Texture = cc->mTarget->texture_ptr;

            _cam->setViewport(Texture->GetWidth(), Texture->GetHeight());
        }
        else {
            Engine::Window* win = engine_ptr->GetWindow();
            cc->ResizeTarget(win->GetWindowWidth(), win->GetWindowHeight());
            _cam->setViewport(win->GetWindowWidth(), win->GetWindowHeight());
        }

        ZSVIEWPORT vp = _cam->getViewport();

        Mat4 Proj = _cam->getProjMatrix();
        Mat4 View = _cam->getViewMatrix();
        Vec3 Pos = _cam->getCameraPosition();
        Vec3 Front = _cam->getCameraFrontVec();

        uint32_t CamSize = 160;

        //Send data to storage buffer
        CamerasStorageBuf->writeDataBuffered(cam_index * CamSize, 64, &Proj);
        CamerasStorageBuf->writeDataBuffered(cam_index * CamSize + 64, 64, &View);
        CamerasStorageBuf->writeDataBuffered(cam_index * CamSize + 128, 12, &Pos);
        CamerasStorageBuf->writeDataBuffered(cam_index * CamSize + 144, 12, &Front);

        
        //GBuffer command buffer
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        vkBeginCommandBuffer(gbuffer_cmdbuf, &beginInfo);

        game_data->vk_main->CurrentCmdBuffer = gbuffer_cmdbuf;

        GBufferRenderPass->SetClearSize(vp.endX, vp.endY);
        GBufferRenderPass->CmdBegin(gbuffer_cmdbuf, gbuffer_fb);

        render_settings.CurrentViewMask = 0xFFFFFFFFFFFFFFFF;

        bool binded = false;

        

        for (unsigned int i = 0; i < ObjectsToRender.size(); i++) {
            VKObjectToRender* obr = &ObjectsToRender[i];
            GameObject* obj = ObjectsToRender[i].obj;
            if (obj->hasMesh() && obr->mat != nullptr) {
                MeshProperty* mesh = obj->getPropertyPtr<MeshProperty>();
                VKMaterialTemplate* MaterialTemplate = (VKMaterialTemplate*)obr->mat->mTemplate;
                ZSVulkanPipeline* Pipeline = MaterialTemplate->Pipeline;
                if (MaterialTemplate->mPipelineCreated) {
                    if (!binded) {
                        Pipeline->CmdBindPipeline(gbuffer_cmdbuf);

                        ((VKMaterialTemplate*)obr->mat->mTemplate)->Pipeline->GetPipelineLayout()
                            ->CmdBindDescriptorSets(gbuffer_cmdbuf, 0, 3);

                        binded = true;
                    }


                    
                    if (Pipeline != nullptr) {
                        VkDescriptorSet set = VK_NULL_HANDLE;

                        if (obr->mat->DescrSetTextures != nullptr) {
                            set = obr->mat->DescrSetTextures->getDescriptorSet();

                            vkCmdBindDescriptorSets(gbuffer_cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                Pipeline->_GetPipelineLayout(), 1,
                                1, &set, 0, nullptr);
                        }
                        //Send object transform
                        Pipeline->CmdPushConstants(gbuffer_cmdbuf, VK_SHADER_STAGE_ALL_GRAPHICS, 0, 8, &obr->TransformArrayIndex);
                        Pipeline->CmdPushConstants(gbuffer_cmdbuf, VK_SHADER_STAGE_ALL_GRAPHICS, 8, 4, &cam_index); 
                        //Send material props
                        if(obr->mat->MaterialBuffer != nullptr)
                            Pipeline->CmdPushConstants(gbuffer_cmdbuf, VK_SHADER_STAGE_FRAGMENT_BIT, 64, MaterialTemplate->BufferSize, obr->mat->MaterialBuffer);

                    }
                    if (mesh->mesh_ptr->resource_state == RESOURCE_STATE::STATE_LOADED)
                        obj->DrawMesh(this);
                }
            }
        }


        vkCmdEndRenderPass(gbuffer_cmdbuf);
        vkEndCommandBuffer(gbuffer_cmdbuf);


        //Deffered cmdbuf
        vkBeginCommandBuffer(deffered_cmdbuf, &beginInfo);

        game_data->vk_main->CurrentCmdBuffer = deffered_cmdbuf;

        DefferedRenderPass->SetClearSize(vp.endX, vp.endY);
        DefferedRenderPass->CmdBegin(deffered_cmdbuf, deffered_fb);

        DefferedPipeline->CmdBindPipeline(deffered_cmdbuf);
        DefferedPipeline->CmdBindDescriptorSets(deffered_cmdbuf);
        DefferedPipeline->CmdPushConstants(deffered_cmdbuf, VK_SHADER_STAGE_ALL_GRAPHICS, 0, 4, &cam_index);
        gbuffer_fb->BindAttachmentsDescrSet(1, deffered_cmdbuf, DefferedPipeline->GetPipelineLayout());

        Engine::getPlaneMesh2D()->Draw();

        vkCmdEndRenderPass(deffered_cmdbuf);
        vkEndCommandBuffer(deffered_cmdbuf);
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
    this->mShadowMapShader->compileFromFile("Shaders/vulkan_test/shadowmap/vert.spv", "", "Shaders/vulkan_test/shadowmap/geom.spv");
    final_shader->compileFromFile("Shaders/vulkan_test/final/vert.spv", "Shaders/vulkan_test/final/frag.spv");

    GBufferRenderPass = new ZSVulkanRenderPass;
    GBufferRenderPass->PushColorAttachment(TextureFormat::FORMAT_RGBA, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    GBufferRenderPass->PushColorAttachment(TextureFormat::FORMAT_RGBA16F, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    GBufferRenderPass->PushColorAttachment(TextureFormat::FORMAT_RGBA16F, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    GBufferRenderPass->PushColorAttachment(TextureFormat::FORMAT_RGBA, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    GBufferRenderPass->PushColorAttachment(TextureFormat::FORMAT_RGBA, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    GBufferRenderPass->PushDepthAttachment();
    GBufferRenderPass->Create();

    DefferedRenderPass = new ZSVulkanRenderPass;
    DefferedRenderPass->PushColorAttachment(TextureFormat::FORMAT_RGBA, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    DefferedRenderPass->PushColorAttachment(TextureFormat::FORMAT_RGBA, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    DefferedRenderPass->Create();

    game_data->vk_main->mGBufferRenderPass = GBufferRenderPass;
    game_data->vk_main->mDefferedRenderPass = DefferedRenderPass;

    mMaterialSampler = new ZSVulkanSampler;
    mMaterialSampler->CreateSampler();
    game_data->vk_main->mDefaultTextureSampler = mMaterialSampler;


    
    TransformStorageBuf = static_cast<vkUniformBuffer*>(allocUniformBuffer());
    TransformStorageBuf->init(0, sizeof(Mat4) * 4000, true);

    SkinningStorageBuf = static_cast<vkUniformBuffer*>(allocUniformBuffer());
    SkinningStorageBuf->init(4, sizeof(Mat4) * 200 * 512, true);

    CamerasStorageBuf = static_cast<vkUniformBuffer*>(allocUniformBuffer());
    CamerasStorageBuf->init(2, 160 * 10, true);

   

    ShadowRenderPass = new ZSVulkanRenderPass;
    ShadowRenderPass->PushDepthAttachment();
    ShadowRenderPass->Create();
    ShadowRenderPass->SetClearSize(4096, 4096);
    

    ShadowFb = new ZSVulkanFramebuffer;
    ShadowFb->AddDepth(4096, 4096, 4, TextureFormat::FORMAT_DEPTH_32);
    ShadowFb->SetLayersCount(4);
    ShadowFb->Create(ShadowRenderPass);


    Engine::ZsVkPipelineConf Conf;
    Conf.hasDepth = false;
    Conf.cullFace = false;
    Conf.LayoutInfo.DescrSetLayout->pushStorageBuffer(GetCamerasStorageBuffer(), VK_SHADER_STAGE_ALL_GRAPHICS);
    Conf.LayoutInfo.DescrSetLayout->pushUniformBuffer((Engine::vkUniformBuffer*)this->lightsBuffer, VK_SHADER_STAGE_FRAGMENT_BIT);
    Conf.LayoutInfo.DescrSetLayoutSampler->pushImageSamplers(0, 5);
    Conf.LayoutInfo.AddPushConstant(4, VK_SHADER_STAGE_ALL_GRAPHICS);
   
    DefferedPipeline = new Engine::ZSVulkanPipeline;
    DefferedPipeline->Create((Engine::vkShader*)deffered_light, DefferedRenderPass, Conf);

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

	
	vkCreateSemaphore(game_data->vk_main->mDevice->getVkDevice(), &semaphoreInfo, nullptr, &MaterialsFinishedSemaphore);
    vkCreateSemaphore(game_data->vk_main->mDevice->getVkDevice(), &semaphoreInfo, nullptr, &DefferedFinishedSemaphore);
    vkCreateSemaphore(game_data->vk_main->mDevice->getVkDevice(), &semaphoreInfo, nullptr, &ShadowFinishedSemaphore);
    

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = game_data->vk_main->mDevice->GetGraphicsQueueFamilyIndex();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Optional

    vkCreateCommandPool(game_data->vk_main->mDevice->getVkDevice(), &poolInfo, nullptr, &commandPool);

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)1;

    vkAllocateCommandBuffers(game_data->vk_main->mDevice->getVkDevice(), &allocInfo, &m3dCmdBuf);
    vkAllocateCommandBuffers(game_data->vk_main->mDevice->getVkDevice(), &allocInfo, &mDefferedCmdBuf);
    vkAllocateCommandBuffers(game_data->vk_main->mDevice->getVkDevice(), &allocInfo, &mShadowCmdBuf);

    PresentInfrastructure.Create(commandPool, game_data->vk_main->mSwapChain->GetSwapChainImagesCount());

    Engine::ZsVkPipelineConf ConfFinal;
    ConfFinal.hasDepth = false;
    ConfFinal.cullFace = false;
    ConfFinal.LayoutInfo.DescrSetLayoutSampler->pushImageSampler(0);

    PresentPipeline = new Engine::ZSVulkanPipeline;
    PresentPipeline->Create((Engine::vkShader*)final_shader, PresentInfrastructure.OutRenderPass, ConfFinal);


    if (engine_ptr->desc->game_perspective == PERSP_3D) {
        MtShProps::genDefaultMtShGroup(default3d, mSkyboxShader, mTerrainShader, water_shader, default_particle);
    }

    
}

void Engine::VKRenderer::FillFinalCmdBufs() {
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    CameraComponent* MainCam = static_cast<CameraComponent*>(mMainCameraComponent);
    ZSVulkanFramebuffer* CamFb = static_cast<ZSVulkanFramebuffer*>(MainCam->mDefferedBuffer);
    ZSVulkanRenderPass* renderpass = PresentInfrastructure.OutRenderPass;

    for (uint32_t SW_image = 0; SW_image < game_data->vk_main->mSwapChain->GetSwapChainImagesCount(); SW_image++) {
        VkCommandBuffer cmdbuf = PresentInfrastructure.PresentCmdbufs[SW_image];
        ZSVulkanFramebuffer* fb = PresentInfrastructure.OutFramebuffers[SW_image];
        
        vkBeginCommandBuffer(cmdbuf, &beginInfo);

        game_data->vk_main->CurrentCmdBuffer = cmdbuf;

        renderpass->CmdBegin(cmdbuf, fb);

        PresentPipeline->CmdBindPipeline(cmdbuf);
        CamFb->BindAttachmentsDescrSet(1, cmdbuf, PresentPipeline->GetPipelineLayout());
        Engine::getPlaneMesh2D()->Draw();

        vkCmdEndRenderPass(cmdbuf);
        vkEndCommandBuffer(cmdbuf);

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

    for (unsigned int i = 0; i < ObjectsToRender.size(); i++) {
        VKObjectToRender* obr = &ObjectsToRender[i];
        GameObject* obj = ObjectsToRender[i].obj;
        if (obj->hasMesh() && obr->mat != nullptr) {
            MeshProperty* mesh = obj->getPropertyPtr<MeshProperty>();
       
            //Send object transform
            ShadowPipeline->CmdPushConstants(mShadowCmdBuf, VK_SHADER_STAGE_VERTEX_BIT, 0, 8, &obr->TransformArrayIndex);

            if (mesh->mesh_ptr->resource_state == RESOURCE_STATE::STATE_LOADED)
                obj->DrawMeshInstanced(this, 4);
            
        }
    }


    vkCmdEndRenderPass(mShadowCmdBuf);
    vkEndCommandBuffer(mShadowCmdBuf);
}

void Engine::VKRenderer::ComputeAll() {
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    uint32_t _imageIndex;
    VkResult imageResult = vkAcquireNextImageKHR(game_data->vk_main->mDevice->getVkDevice(),
        game_data->vk_main->mSwapChain->GetSwapChain(), UINT64_MAX, PresentInfrastructure.imageAvailableSemaphore, VK_NULL_HANDLE, &_imageIndex);

    if (imageResult == VK_ERROR_OUT_OF_DATE_KHR || imageResult == VK_SUBOPTIMAL_KHR) {
        this->PresentInfrastructure.RecreateSwapchain();
        _imageIndex = 0;
        //return;
    }

    VkSemaphore Wait = PresentInfrastructure.imageAvailableSemaphore;

    for (uint32_t Camera_i = 0; Camera_i < CamerasToRender.size(); Camera_i++) {

        VKCameraToRender* CR = &CamerasToRender[Camera_i];

        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &Wait;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &CR->GBufferCmdBuf;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &MaterialsFinishedSemaphore;
        vkQueueSubmit(game_data->vk_main->mDevice->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);

        
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &MaterialsFinishedSemaphore;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &CR->DefferedCmdBuf;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &DefferedFinishedSemaphore;
        vkQueueSubmit(game_data->vk_main->mDevice->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);

        Wait = DefferedFinishedSemaphore;
    }

    VkCommandBuffer final_cmdbuf = PresentInfrastructure.PresentCmdbufs[_imageIndex];

    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &Wait;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &final_cmdbuf;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &PresentInfrastructure.PresentBeginSemaphore;
    vkQueueSubmit(game_data->vk_main->mDevice->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);



    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &PresentInfrastructure.PresentBeginSemaphore;

    VkSwapchainKHR swapChains[] = { game_data->vk_main->mSwapChain->GetSwapChain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    //uint32_t imageIndex = 0;
    presentInfo.pImageIndices = &_imageIndex;
    presentInfo.pResults = nullptr; // Optional
    vkQueuePresentKHR(game_data->vk_main->mDevice->GetPresentQueue(), &presentInfo);

}

void Engine::VKRenderer::SetViewport(VkCommandBuffer cmdbuf, unsigned int startX, unsigned int startY, unsigned int width, unsigned int height) {
    VkViewport viewport = {startX, startY, width, height, 0, 1};
    vkCmdSetViewport(cmdbuf, 0, 1, &viewport);
}