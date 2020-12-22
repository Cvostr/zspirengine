#ifndef ENGINE_H
#define ENGINE_H

#include "vulkan/ZSVulkanInstance.hpp"
#include "vulkan/ZSVulkanDevice.hpp"
#include "vulkan/ZSVulkanSwapchain.hpp"

#include "engine/EngineComponentManager.hpp"
#include "engine/Window.hpp"
#include "engine_types.h"


class ZSpireEngine
{
private:
    void* zsgame_ptr;
    bool gameRuns;

    Engine::ZSVulkanInstance* mVkInstance;
    Engine::ZSVulkanDevice* mVkDevice;
    Engine::ZSVulkanSwapChain* mVkSwapChain;

    Engine::EngineComponentManager* mComponentManager;
    Engine::Window* mWindow;
public:
    ZSGAME_DESC* desc;
    ZSENGINE_CREATE_INFO* engine_info;

    ZSpireEngine(ZSENGINE_CREATE_INFO* info, Engine::ZSWINDOW_CREATE_INFO* win, ZSGAME_DESC* desc);
    ZSpireEngine();

    void* getGameDataPtr();
    Engine::Window* GetWindow() { return mWindow; }
    void SetWindow(Engine::Window* win) { mWindow = win; }

    Engine::ZSVulkanDevice* GetDevice() { return mVkDevice; }

    void loadGame();
};

#endif 
