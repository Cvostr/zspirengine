#include "../headers/engine.h"
#include "../headers/engine/loader.h"
#include "../headers/game.h"
#include <GL/glew.h>
#include <iostream>
#include "../headers/misc/oal_manager.h"
#include "../headers/input/zs-input.h"

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
       
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        //OpenGL 4.2
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_WIN_MODE = SDL_WINDOW_OPENGL; //Set window mode to OpenGL

        std::cout << "OpenGL version : 4.3 core" << std::endl;
       
        SDL_DisplayMode current;
        SDL_GetCurrentDisplayMode(0, &current);

        this->window = SDL_CreateWindow(win->title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win->Width, win->Height, SDL_WIN_MODE); //Create window

            this->glcontext = SDL_GL_CreateContext(window);
            glViewport(0, 0, win->Width, win->Height);

            glewExperimental = GL_TRUE;
            if(glewInit() != GLEW_OK) std::cout << "GLEW initialize failed" << std::endl; else {
                std::cout << "GLEW initialize sucessful" << std::endl;
            }

        
        //initialize OpenAL sound system
        Engine::SFX::initAL();

    }
}

SDL_Window* ZSpireEngine::getWindowSDL(){
    return this->window;
}

void* ZSpireEngine::getGameDataPtr(){
    return this->zsgame_ptr;
}

void ZSpireEngine::startManager(IEngineComponent* component){
    component->setDpMetrics(this->window_info->Width, this->window_info->Height);
    component->game_desc_ptr = this->desc;
    component->init();
    this->components.push_back(component);
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
    //Allocate Glyph manager
    game_data->glyph_manager = new GlyphManager;
    startManager(game_data->glyph_manager);
    //Allocate script manager
    game_data->script_manager = new Engine::AGScriptMgr;

    game_data->out_manager = new Engine::OutputManager;
    game_data->out_manager->consoleLogWorking = true;

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
    //Load all resources
    game_data->resources->loadResourcesTable(this->desc->resource_map_file_path);


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

    static uint64_t NOW = SDL_GetPerformanceCounter();
    static uint64_t last = 0;

    while(gameRuns == true){

        last = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = (NOW - last) / 1000.f;
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
    game_data->world->clear(); //clear world
    Engine::Loader::stop();
    Engine::SFX::destroyAL();
    //Destroys all created managers
    destroyAllManagers();
    delete game_data;
    SDL_DestroyWindow(window); //Destroy SDL and opengl
    SDL_GL_DeleteContext(glcontext);

}

void ZSpireEngine::destroyAllManagers(){
    //we must do that in reverse order
    for(int i = static_cast<int>(components.size()) - 1; i >= 0; i --){
        delete components[i];
    }
}
