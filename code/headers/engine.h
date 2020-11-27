#ifndef ENGINE_H
#define ENGINE_H

#include <SDL2/SDL.h>

#include "vulkan/ZSVulkanInstance.hpp"
#include "vulkan/ZSVulkanDevice.hpp"
#include "vulkan/ZSVulkanSwapchain.hpp"

#include "engine/EngineComponent.h"
#include "engine/engine_window.h"
#include "engine_types.h"


class ZSpireEngine
{
private:
    void* zsgame_ptr;
    float deltaTime;
    bool gameRuns;

    SDL_Window* mWindow;
    SDL_GLContext mGLContext;

    Engine::ZSVulkanInstance* mVkInstance;
    Engine::ZSVulkanDevice* mVkDevice;
    Engine::ZSVulkanSwapChain* mVkSwapChain;

    std::vector<IEngineComponent*> components;
public:
    ZSGAME_DESC* desc;
    ZSENGINE_CREATE_INFO* engine_info;
    ZSWINDOW_CREATE_INFO* window_info;

    ZSpireEngine(ZSENGINE_CREATE_INFO* info, ZSWINDOW_CREATE_INFO* win, ZSGAME_DESC* desc);
    ZSpireEngine();

    SDL_Window* getWindowSDL();
    void* getGameDataPtr();

    void startManager(IEngineComponent* component);
    void updateDeltaTime(float deltaTime);
    void updateResolution(int W, int H);
    void setWindowMode(unsigned int mode);
    void destroyAllManagers();

    void loadGame();
};

#endif // ENGINE_H
