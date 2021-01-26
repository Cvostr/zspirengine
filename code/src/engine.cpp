#include "../headers/engine.h"
#include "../headers/engine/BackgroundLoader.hpp"
#include "../headers/game.h"
#include <GL/glew.h>
#include <iostream>
#include "../headers/input/Input.hpp"
#include "../headers/engine/Logger.hpp"
#include "../headers/ogl/GLRenderer.hpp"
#include "../headers/vulkan/VKRenderer.hpp"

ZSpireEngine* engine_ptr;
ZSGAME_DATA* game_data;

using namespace Engine;

ZSpireEngine::ZSpireEngine(){

}

ZSpireEngine::ZSpireEngine(ZSENGINE_CREATE_INFO* info, Engine::ZSWINDOW_CREATE_INFO* win, ZSGAME_DESC* desc)
{
    engine_ptr = this;
    game_data = new ZSGAME_DATA;
    mWindow = new Engine::Window;
    Logger::Log(LogType::LOG_TYPE_INFO) << "ZSPIRE Engine v0.1\n";
    //Store info structures
    this->desc = desc;
    this->engine_info = info;

    if (info->createWindow == true) { //we have to init window
        
        mWindow->CreateWindow(win, info);
        
        if (info->graphicsApi == VULKAN) {
            game_data->vk_main = new Engine::ZSVulkan;

            game_data->vk_main->mInstance = new Engine::ZSVulkanInstance;
            game_data->vk_main->mInstance->init(true, desc->app_label.c_str(), desc->app_version, mWindow->GetWindowSDL());
            game_data->vk_main->mDevice = CreatePrimaryDevice(game_data->vk_main->mInstance);
            game_data->vk_main->mSwapChain = new Engine::ZSVulkanSwapChain;
            game_data->vk_main->mSwapChain->initSwapchain(game_data->vk_main->mDevice,
                                                          game_data->vk_main->mInstance, 
                                                          win->Width, win->Height);
            game_data->vk_main->mVMA = new Engine::ZSVMA(game_data->vk_main->mInstance,
                                                         game_data->vk_main->mDevice);
        }
    }
}

void* ZSpireEngine::getGameDataPtr(){
    return this->zsgame_ptr;
}


void ZSpireEngine::loadGame(){
    gameRuns = true;

    mComponentManager = new Engine::EngineComponentManager;
    mWindow->SetComponentManager(mComponentManager);

    game_data->window = this->mWindow;

    //Allocate pipeline and start it as manager
    if(engine_info->graphicsApi == VULKAN)
        game_data->pipeline = new Engine::VKRenderer;
    else
        game_data->pipeline = new Engine::GLRenderer;
    mComponentManager->startManager(game_data->pipeline);

    //Allocate resource manager
    game_data->resources = new Engine::ResourceManager;
    //Start it as manager
    mComponentManager->startManager(game_data->resources);
    //Allocate Glyph manager
    game_data->glyph_manager = new GlyphManager;
    mComponentManager->startManager(game_data->glyph_manager);
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

    Loader::start();
    Loader::setBlobRootDirectory(this->desc->blob_root_path);
    //Load all resources
    game_data->resources->loadResourcesTable(this->desc->resource_map_file_path);
    //Compile script
    game_data->script_manager->AddScriptFiles();

    SceneResource* world_resource = game_data->resources->getResource<SceneResource>(desc->startup_scene);
    //check, if World resource found
    if (world_resource) {
        world_resource->load();
        game_data->world->loadFromMemory((const char*)world_resource->data, world_resource->size, game_data->pipeline->getRenderSettings());
    }
    //call onStart on all objects
    game_data->world->call_onStart();

    while(gameRuns == true){

        game_data->time->Tick();

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

        mWindow->SwapGL();

    }
    mWindow->DestroyWindow();
    game_data->world->clear(); //clear world
    Loader::stop();
    //Destroys all created managers
    mComponentManager->destroyAllManagers();
    delete game_data;
}
