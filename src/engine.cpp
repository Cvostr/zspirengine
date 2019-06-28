#include "../headers/engine.h"
#include "../headers/engine/loader.h"
#include "../headers/game.h"
#include <GL/glew.h>
#include <iostream>
#include "../headers/misc/oal_manager.h"

ZSpireEngine* engine_ptr;

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
            //OpenGL 3.2
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
            SDL_WIN_MODE = SDL_WINDOW_OPENGL; //Set window mode to OpenGL

            std::cout << "OpenGL version : 3.2 core" << std::endl;
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

void ZSpireEngine::loadGame(){
    gameRuns = true;

    ZSGAME_DATA* data = new ZSGAME_DATA;
    this->zsgame_ptr = static_cast<void*>(data);
    //Allocate pipeline and start it as manager
    data->pipeline = new Engine::RenderPipeline;
    startManager(data->pipeline);
    //Allocate resource manager
    data->resources = new Engine::ResourceManager;
    //Start it as manager
    startManager(data->resources);
    data->world = new Engine::World(data->resources);

    switch(this->desc->game_perspective){
        case PERSP_2D:{ //2D project

            data->world->getCameraPtr()->setProjectionType(ZSCAMERA_PROJECTION_ORTHOGONAL);
            data->world->getCameraPtr()->setPosition(ZSVECTOR3(0,0,0));
            data->world->getCameraPtr()->setFront(ZSVECTOR3(0,0,1));
            break;
        }
        case PERSP_3D:{ //3D project
            data->world->getCameraPtr()->setProjectionType(ZSCAMERA_PROJECTION_PERSPECTIVE);
            data->world->getCameraPtr()->setPosition(ZSVECTOR3(0,0,0));
            data->world->getCameraPtr()->setFront(ZSVECTOR3(0,0,1));
            data->world->getCameraPtr()->setZplanes(1, 2000);
            break;
        }
    }

    Engine::Loader::start();
    Engine::Loader::setBlobRootDirectory(this->desc->blob_root_path);

    data->resources->loadResourcesTable(this->desc->resource_map_file_path);
    data->world->loadFromFile(desc->game_dir + "/" + desc->startup_scene);

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
        }

        data->pipeline->render();

        SDL_GL_SwapWindow(window); //Send rendered frame

    }
    Engine::Loader::stop();
    Engine::SFX::destroyAL();
    destroyAllManagers();
    SDL_DestroyWindow(window); //Destroy SDL and opengl
    SDL_GL_DeleteContext(glcontext);
}

void ZSpireEngine::destroyAllManagers(){
    //we must do that in reverse order
    for(int i = static_cast<int>(components.size()) - 1; i >= 0; i --){
        delete components[i];
    }
}
