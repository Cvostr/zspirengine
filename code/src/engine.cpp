#include "../headers/engine.h"
#include "../headers/engine/BackgroundLoader.hpp"
#include "../headers/game.h"
#include <GL/glew.h>
#include <iostream>
#include "../headers/input/zs-input.h"
#include "../headers/ogl/GLRenderer.hpp"
#include "../headers/vulkan/VKRenderer.hpp"

ZSpireEngine* engine_ptr;
ZSGAME_DATA* game_data;

ZSpireEngine::ZSpireEngine(){

}

ZSpireEngine::ZSpireEngine(ZSENGINE_CREATE_INFO* info, ZSWINDOW_CREATE_INFO* win, ZSGAME_DESC* desc)
{
    engine_ptr = this;
    game_data = new ZSGAME_DATA;
    std::cout << "ZSPIRE Engine v0.1" << std::endl;
    //Store info structures
    this->desc = desc;
    this->engine_info = info;
    this->window_info = win;

    if (info->createWindow == true) { //we have to init window
        std::cout << "Opening SDL2 window" << std::endl;
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) //Trying to init SDL
        {
            std::cout << "Error opening window " << SDL_GetError() << std::endl;
        }

        unsigned int SDL_WIN_MODE = 0;
        if (info->graphicsApi == OGL){
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
            SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
            //OpenGL 4.3
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
            SDL_WIN_MODE = SDL_WINDOW_OPENGL; //Set window mode to OpenGL
            std::cout << "OpenGL version : 4.3 core" << std::endl;
        } else if (info->graphicsApi == VULKAN) {
            SDL_WIN_MODE = SDL_WINDOW_VULKAN;
        }

        SDL_DisplayMode current;
        SDL_GetCurrentDisplayMode(0, &current);

        this->mWindow = SDL_CreateWindow(win->title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win->Width, win->Height, SDL_WIN_MODE); //Create window
        if (info->graphicsApi == VULKAN) {
            game_data->vk_main = new Engine::ZSVulkan;

            game_data->vk_main->mInstance = new Engine::ZSVulkanInstance;
            game_data->vk_main->mInstance->init(true, desc->app_label.c_str(), desc->app_version, mWindow);
            game_data->vk_main->mDevice = CreatePrimaryDevice(game_data->vk_main->mInstance);
            game_data->vk_main->mSwapChain = new Engine::ZSVulkanSwapChain;
            game_data->vk_main->mSwapChain->initSwapchain(game_data->vk_main->mDevice,
                                                          game_data->vk_main->mInstance, 
                                                          win->Width, win->Height);
            game_data->vk_main->mVMA = new Engine::ZSVMA(game_data->vk_main->mInstance,
                                                         game_data->vk_main->mDevice);
        }
        else if (info->graphicsApi == OGL) {
            this->mGLContext = SDL_GL_CreateContext(mWindow);
            glViewport(0, 0, win->Width, win->Height);

            glewExperimental = GL_TRUE;
            if (glewInit() != GLEW_OK) std::cout << "GLEW initialize failed" << std::endl; else {
                std::cout << "GLEW initialize sucessful" << std::endl;
            }
        }
    }
}

SDL_Window* ZSpireEngine::getWindowSDL(){
    return this->mWindow;
}

void* ZSpireEngine::getGameDataPtr(){
    return this->zsgame_ptr;
}

void ZSpireEngine::startManager(IEngineComponent* component){
    component->setDpMetrics(this->window_info->Width, this->window_info->Height);
    component->game_desc_ptr = this->desc;
    component->OnCreate();
    this->components.push_back(component);
}
void ZSpireEngine::updateDeltaTime(float deltaTime){
    this->deltaTime = deltaTime;

    for(unsigned int i = 0; i < components.size(); i ++){
        components[i]->deltaTime = deltaTime;
    }
}

void ZSpireEngine::updateResolution(int W, int H){
    SDL_SetWindowSize(this->mWindow, W, H);
    for(unsigned int i = 0; i < components.size(); i ++){
        components[i]->OnUpdateWindowSize(W, H);
    }
}

void ZSpireEngine::setWindowMode(unsigned int mode){
    SDL_SetWindowFullscreen(this->mWindow, mode);
}

void ZSpireEngine::loadGame(){
    gameRuns = true;

    this->zsgame_ptr = static_cast<void*>(game_data);
    //Allocate pipeline and start it as manager
    if(engine_info->graphicsApi == VULKAN)
        game_data->pipeline = new Engine::VKRenderer;
    else
        game_data->pipeline = new Engine::GLRenderer;
    startManager(game_data->pipeline);
    //Allocate resource manager
    game_data->resources = new Engine::ResourceManager;
    //Start it as manager
    startManager(game_data->resources);
    //Allocate Glyph manager
    game_data->glyph_manager = new GlyphManager;
    startManager(game_data->glyph_manager);
    //Allocate script manager
    game_data->script_manager = new Engine::AGScriptMgr;
    //Allocate output manager
    game_data->out_manager = new Engine::OutputManager;
    game_data->out_manager->consoleLogWorking = true;

    game_data->ui_manager = new Engine::UiManager;
    //Allocate OpenAL manager
    game_data->oal_manager = new Engine::OALManager;
    game_data->oal_manager->initAL();
    //Allocate time manager
    game_data->time = new Engine::Time;

    game_data->world = new Engine::World();


    switch(this->desc->game_perspective){
        case PERSP_2D:{ //2D project

            game_data->world->getCameraPtr()->setProjectionType(ZSCAMERA_PROJECTION_ORTHOGONAL);
            game_data->world->getCameraPtr()->setPosition(Vec3(0,0,0));
            game_data->world->getCameraPtr()->setFront(Vec3(0,0,1));
            break;
        }
        case PERSP_3D:{ //3D project
            game_data->world->getCameraPtr()->setProjectionType(ZSCAMERA_PROJECTION_PERSPECTIVE);
            game_data->world->getCameraPtr()->setPosition(Vec3(0,0,0));
            game_data->world->getCameraPtr()->setFront(Vec3(0,0,1));
            game_data->world->getCameraPtr()->setZplanes(1, 2000);
            break;
        }
    }

    Engine::Loader::start();
    Engine::Loader::setBlobRootDirectory(this->desc->blob_root_path);
    //Load all resources
    game_data->resources->loadResourcesTable(this->desc->resource_map_file_path);
    //Compile script
    game_data->script_manager->AddScriptFiles();

    Engine::ZsResource* world_resource = game_data->resources->getResource<Engine::ZsResource>(desc->startup_scene);
    //check, if World resource found
    if (world_resource) {
        world_resource->request = new Engine::Loader::LoadRequest;
        world_resource->request->offset = world_resource->offset;
        world_resource->request->size = world_resource->size;
        world_resource->request->file_path = world_resource->blob_path;
        loadImmideately(world_resource->request);

        game_data->world->loadFromMemory((const char*)world_resource->request->data, world_resource->request->size, game_data->pipeline->getRenderSettings());
    }
    //call onStart on all objects
    game_data->world->call_onStart();

    while(gameRuns == true){

        game_data->time->Tick();
        updateDeltaTime(game_data->time->GetDeltaTime());

        SDL_Event event;
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT)  //If user caused SDL window to close
                this->gameRuns = false;
            //Update input state from SDL event
            Input::processEventsSDL(&event);

        }
        game_data->world->physical_world->stepSimulation(game_data->time->GetDeltaTime());
        game_data->pipeline->render();

        Input::clearMouseState();
        Input::clearPressedKeys();

        SDL_GL_SwapWindow(mWindow); //Send rendered frame

    }
    game_data->world->clear(); //clear world
    Engine::Loader::stop();
    //Destroys all created managers
    destroyAllManagers();
    delete game_data;
    SDL_DestroyWindow(mWindow); //Destroy SDL and opengl
    SDL_GL_DeleteContext(mGLContext);

}

void ZSpireEngine::destroyAllManagers(){
    //we must do that in reverse order
    for(int i = static_cast<int>(components.size()) - 1; i >= 0; i --){
        delete components[i];
    }
}
