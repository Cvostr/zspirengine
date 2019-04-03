#include "../headers/engine.h"
#include "../headers/game.h"
#include <GL/glew.h>
#include <iostream>

ZSpireEngine::ZSpireEngine(ZSENGINE_CREATE_INFO* info, ZSWINDOW_CREATE_INFO* win, ZSGAME_DESC* desc)
{
    std::cout << "ZSPIRE Engine v0.1" << std::endl;

    this->desc = desc;

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
        }
        SDL_DisplayMode current;
        SDL_GetCurrentDisplayMode(0, &current);

        this->window = SDL_CreateWindow(win->title, 0, 0, win->Width, win->Height, SDL_WIN_MODE); //Create window
        if(info->graphicsApi == OGL32){
            this->glcontext = SDL_GL_CreateContext(window);

            glewExperimental = true;
            if(!glewInit()) std::cout << "GLEW initialize failed" << std::endl;
        }
        if(info->graphicsApi == VULKAN){
            this->vkcontext.init(desc->app_label.c_str(), desc->app_version);
        }
    }
}

void ZSpireEngine::loadGame(){
    gameRuns = true;

    ZSGAME_DATA* data = new ZSGAME_DATA;
    this->zsgame_ptr = static_cast<void*>(data);

    data->pipeline = new EngineRenderPipeline;
    this->resources = new ResourceManager;
    data->world = new Engine::World;

    data->world->loadFromFile(desc->game_dir + "/" + desc->startup_scene);

    while(gameRuns == true){

        SDL_Event event;
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT)  //If user caused SDL window to close
                this->gameRuns = false;
        }

    }
    SDL_DestroyWindow(window); //Destroy SDL and opengl
}
