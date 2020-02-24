#include "../headers/engine.h"
#include "../headers/engine/loader.h"
#include "../headers/game.h"
#include <GL/glew.h>
#include <iostream>
#include "../headers/misc/oal_manager.h"
#include "../headers/Scripting/zsensdk.h"

ZSpireEngine* engine_ptr;
ZSGAME_DATA* game_data;

ZSpireEngine::ZSpireEngine(){

}

ZSpireEngine::ZSpireEngine(ZSENGINE_CREATE_INFO* info, ZSWINDOW_CREATE_INFO* win, ZSGAME_DESC* desc)
{
    engine_ptr = this;

    std::cout << "ZSPIRE Engine v0.1" << std::endl;
    //Store info structures
    this->desc = desc;
    this->engine_info = info;
    this->window_info = win;

    if(info->createWindow == true){ //we have to init window
        std::cout << "Opening SDL2 window" << std::endl;
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) //Trying to init SDL
        {
            std::cout << "Error opening window " << SDL_GetError() << std::endl;
        }

        unsigned int SDL_WIN_MODE = 0;
        if(info->graphicsApi == OGL32){ //We'll use opengl to render graphics
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
            SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
            //OpenGL 4.2
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
            SDL_WIN_MODE = SDL_WINDOW_OPENGL; //Set window mode to OpenGL

            std::cout << "OpenGL version : 4.2 core" << std::endl;
        }else{
            SDL_WIN_MODE = SDL_WINDOW_VULKAN; //Set window mode to OpenGL
        }
        SDL_DisplayMode current;
        SDL_GetCurrentDisplayMode(0, &current);

        this->window = SDL_CreateWindow(win->title, 0, 0, win->Width, win->Height, SDL_WIN_MODE); //Create window
        if(info->graphicsApi == OGL32){
            this->glcontext = SDL_GL_CreateContext(window);
            glViewport(0, 0, win->Width, win->Height);

            glewExperimental = GL_TRUE;
            if(glewInit() != GLEW_OK) std::cout << "GLEW initialize failed" << std::endl; else {
                std::cout << "GLEW initialize sucessful" << std::endl;
            }
        }
        if(info->graphicsApi == VULKAN){
            this->vkcontext.init(true, desc->app_label.c_str(), desc->app_version, this->window, win);
        }
        //initialize OpenAL sound system
        Engine::SFX::initAL();

    }
}

ZsVulkan* ZSpireEngine::getVulkanContext(){
    return &this->vkcontext;
}

SDL_Window* ZSpireEngine::getWindowSDL(){
    return this->window;
}

void* ZSpireEngine::getGameDataPtr(){
    return this->zsgame_ptr;
}

void ZSpireEngine::startManager(EngineComponentManager* manager){
    manager->setDpMetrics(this->window_info->Width, this->window_info->Height);
    manager->game_desc_ptr = this->desc;
    manager->init();
    this->components.push_back(manager);
}
void ZSpireEngine::updateDeltaTime(float deltaTime){
    this->deltaTime = deltaTime;

    for(unsigned int i = 0; i < components.size(); i ++){
        components[i]->deltaTime = deltaTime;
    }
}

void ZSpireEngine::updateResolution(int W, int H){
    SDL_SetWindowSize(this->window, W, H);
    for(unsigned int i = 0; i < components.size(); i ++){
        components[i]->updateWindowSize(W, H);
    }
}

void ZSpireEngine::setWindowMode(unsigned int mode){
    SDL_SetWindowFullscreen(this->window, mode);
}

void ZSpireEngine::loadGame(){
    gameRuns = true;

    game_data = new ZSGAME_DATA;
    this->zsgame_ptr = static_cast<void*>(game_data);
    //Allocate pipeline and start it as manager
    game_data->pipeline = new Engine::RenderPipeline;
    startManager(game_data->pipeline);
    //Allocate resource manager
    game_data->resources = new Engine::ResourceManager;
    //Start it as manager
    startManager(game_data->resources);

    game_data->glyph_manager = new GlyphManager;
    startManager(game_data->glyph_manager);

    game_data->world = new Engine::World();

    switch(this->desc->game_perspective){
        case PERSP_2D:{ //2D project

            game_data->world->getCameraPtr()->setProjectionType(ZSCAMERA_PROJECTION_ORTHOGONAL);
            game_data->world->getCameraPtr()->setPosition(ZSVECTOR3(0,0,0));
            game_data->world->getCameraPtr()->setFront(ZSVECTOR3(0,0,1));
            break;
        }
        case PERSP_3D:{ //3D project
            game_data->world->getCameraPtr()->setProjectionType(ZSCAMERA_PROJECTION_PERSPECTIVE);
            game_data->world->getCameraPtr()->setPosition(ZSVECTOR3(0,0,0));
            game_data->world->getCameraPtr()->setFront(ZSVECTOR3(0,0,1));
            game_data->world->getCameraPtr()->setZplanes(1, 2000);
            break;
        }
    }

    Engine::Loader::start();
    Engine::Loader::setBlobRootDirectory(this->desc->blob_root_path);

    game_data->resources->loadResourcesTable(this->desc->resource_map_file_path);
    game_data->world->loadFromFile(desc->game_dir + "/" + desc->startup_scene, game_data->pipeline->getRenderSettings());

    static uint64_t NOW = SDL_GetPerformanceCounter();
    static uint64_t last = 0;

    while(gameRuns == true){

        last = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = (NOW - last) * 1000 / SDL_GetPerformanceFrequency();
        updateDeltaTime(deltaTime);

        SDL_Event event;
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT)  //If user caused SDL window to close
                this->gameRuns = false;
            //Update input state from SDL event
            Input::processEventsSDL(&event);

        }
        game_data->world->physical_world->stepSimulation(deltaTime);
        game_data->pipeline->render();

        Input::clearMouseState();
        Input::clearPressedKeys();

        SDL_GL_SwapWindow(window); //Send rendered frame

    }
    Engine::Loader::stop();
    Engine::SFX::destroyAL();
    destroyAllManagers();
    SDL_DestroyWindow(window); //Destroy SDL and opengl
    SDL_GL_DeleteContext(glcontext);
/*
    Engine::Shader* vert = Engine::allocShader();
    vert->compileFromFile("Shaders/vk_demo/vert.spv", "Shaders/vk_demo/frag.spv");

    ZsVkFrameBuffer fb;
    fb.create(static_cast<Engine::_vk_Shader*>(vert)->pipeline);

    VkCommandPool commandPool;

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = getVulkanContext()->getFamilyIndices()->graphicsIndex;
    poolInfo.flags = 0; // Optional

    vkCreateCommandPool(getVulkanContext()->getVkDevice(), &poolInfo, nullptr, &commandPool);


    VkCommandBuffer cmdbuf;

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) 1;

    vkAllocateCommandBuffers(getVulkanContext()->getVkDevice(), &allocInfo, &cmdbuf);


    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    vkBeginCommandBuffer(cmdbuf, &beginInfo);


    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = static_cast<Engine::_vk_Shader*>(vert)->pipeline->getRenderPass();
    renderPassInfo.framebuffer = fb.getFramebuffer();

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = getVulkanContext()->getSwapChain()->swap_extend;

    VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(cmdbuf, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, static_cast<Engine::_vk_Shader*>(vert)->pipeline->getPipeline());
    vkCmdDraw(cmdbuf, 3, 1, 0, 0);
    vkCmdEndRenderPass(cmdbuf);
    vkEndCommandBuffer(cmdbuf);


    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    vkCreateSemaphore(getVulkanContext()->getVkDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphore);
    vkCreateSemaphore(getVulkanContext()->getVkDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphore);

    while(gameRuns == true){

        uint32_t _imageIndex;
        vkAcquireNextImageKHR(getVulkanContext()->getVkDevice(), getVulkanContext()->getSwapChain()->vk_swapchain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &_imageIndex);




        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmdbuf;

        VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkQueueSubmit(getVulkanContext()->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);


        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {getVulkanContext()->getSwapChain()->vk_swapchain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        uint32_t imageIndex = 0;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr; // Optional
        vkQueuePresentKHR(getVulkanContext()->getPresentQueue(), &presentInfo);
    }*/
}

void ZSpireEngine::destroyAllManagers(){
    //we must do that in reverse order
    for(int i = static_cast<int>(components.size()) - 1; i >= 0; i --){
        delete components[i];
    }
}
