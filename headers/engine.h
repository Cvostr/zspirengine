#ifndef ENGINE_H
#define ENGINE_H

#include <SDL2/SDL.h>

#include "vulkan/zsvulkan.h"
#include "engine/EngineManager.h"
#include "engine/engine_window.h"
#include "engine_types.h"


class ZSpireEngine
{
private:
    void* zsgame_ptr;
    float deltaTime;
    bool gameRuns;

    SDL_Window* window;
    SDL_GLContext glcontext;
    ZsVulkan vkcontext;

    std::vector<EngineComponentManager*> components;
public:
    ZSGAME_DESC* desc;
    ZSENGINE_CREATE_INFO* engine_info;
    ZSWINDOW_CREATE_INFO* window_info;

    ZSpireEngine(ZSENGINE_CREATE_INFO* info, ZSWINDOW_CREATE_INFO* win, ZSGAME_DESC* desc);
    ZSpireEngine();

    SDL_Window* getWindowSDL();
    ZsVulkan* getVulkanContext();
    void* getGameDataPtr();

    void startManager(EngineComponentManager* manager);
    void updateDeltaTime(float deltaTime);
    void updateResolution(int W, int H);
    void setWindowMode(unsigned int mode);
    void destroyAllManagers();

    void loadGame();
};

#endif // ENGINE_H
