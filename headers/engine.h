#ifndef ENGINE_H
#define ENGINE_H

#include <string>
#include <cstdint>

#include <SDL2/SDL.h>

#include "vulkan/zsvulkan.h"
#include "engine/EngineManager.h"
#include "engine/engine_window.h"

enum ZSGAPI {OGL32, VULKAN};
enum ZSPERSPECTIVE {PERSP_3D = 3, PERSP_2D = 2};

typedef struct ZSENGINE_CREATE_INFO{
    char* appName; //String to store name of application
    bool createWindow; //will engine create SDL window at start
    ZSGAPI graphicsApi; //Selected graphics API
}ZSENGINE_CREATE_INFO;

typedef struct ZSGAME_DESC{
    std::string app_label;
    int app_version;

    std::string game_dir; //Game root directory
    std::string blob_root_path; //Relative path to directory with blobs
    std::string resource_map_file_path; //Relative path to resource map
    ZSPERSPECTIVE game_perspective; //Perspective of game scenes
    std::string startup_scene; //Relative path to scene, what loaded on startup

}ZSGAME_DESC;

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

    SDL_Window* getWindowSDL();
    ZsVulkan* getVulkanContext();
    void* getGameDataPtr();

    void startManager(EngineComponentManager* manager);
    void updateDeltaTime(float deltaTime);

    void loadGame();
};

#endif // ENGINE_H
