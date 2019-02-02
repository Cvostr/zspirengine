#ifndef ENGINE_H
#define ENGINE_H

#include <string>
#include <cstdint>

#include <SDL2/SDL.h>
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
}ZSWINDOW_CREATE_INFO;

class ZSpireEngine
{
private:
    SDL_Window* window;
public:
    ZSpireEngine(ZSENGINE_CREATE_INFO* info, ZSWINDOW_CREATE_INFO* win);
};

#endif // ENGINE_H
