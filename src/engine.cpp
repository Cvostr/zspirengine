#include "../headers/engine.h"
#include <iostream>

ZSpireEngine::ZSpireEngine(ZSENGINE_CREATE_INFO* info, ZSWINDOW_CREATE_INFO* win)
{
    std::cout << "ZSPIRE Engine v0.1";

    if(info->createWindow == true){ //we have to init window
        std::cout << "Opening SDL2 window";
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) //Trying to init SDL
        {
            std::cout << "Error opening window " << SDL_GetError();
        }
    }
}
