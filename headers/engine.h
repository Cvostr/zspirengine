#ifndef ENGINE_H
#define ENGINE_H

#include <string>
#include <cstdint>

#include <SDL2/SDL.h>

#include "vulkan/zsvulkan.h"

enum ZSGAPI {OGL32, VULKAN};

typedef struct ZSENGINE_CREATE_INFO{
    char* appName; //String to store name of application
    bool createWindow; //will engine create SDL window at start
    ZSGAPI graphicsApi; //Selected graphics API
}ZSENGINE_CREATE_INFO;

typedef struct ZSWINDOW_CREATE_INFO{
    char* title; //title for window
    int Width;
    int Height;
    bool resizable;

    ZSWINDOW_CREATE_INFO(){
        Width = 640;
        Height = 480;
        resizable = true;
    }
}ZSWINDOW_CREATE_INFO;

typedef struct ZSGAME_DESC{
    char* game_dir;
    int game_perspective;
}ZSGAME_DESC;

class ZSpireEngine
{
private:
    SDL_Window* window;
    SDL_GLContext glcontext;
    ZsVulkan vkcontext;
public:
    ZSpireEngine(ZSENGINE_CREATE_INFO* info, ZSWINDOW_CREATE_INFO* win);

    void setGame(ZSGAME_DESC* desc);
};

#endif // ENGINE_H
