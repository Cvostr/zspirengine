#ifndef ENGINE_H
#define ENGINE_H

#include <string>
#include <cstdint>

#include <SDL2/SDL.h>

#include "vulkan/zsvulkan.h"
#include "render/zs-pipeline.h"
#include "engine/resources.h"
#include "world/World.h"

enum ZSGAPI {OGL32, VULKAN};
enum ZSRESTYPE {TYPE_BLOBS, TYPE_FILES};
enum ZSPERSPECTIVE {PERSP_3D, PERSP_2D};

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
    std::string app_label;
    int app_version;

    std::string game_dir; //Game root directory
    std::string resource_map_file_path; //Relative path to resource map (if blob mode)
    ZSPERSPECTIVE game_perspective; //Perspective of game scenes
    ZSRESTYPE resource_type; //Type of resource store
    std::string startup_scene; //Relative path to scene, what loaded on startup

}ZSGAME_DESC;

class ZSpireEngine
{
private:
    Engine::World* world;
    EngineRenderPipeline* pipeline;
    ResourceManager* resources;

    SDL_Window* window;
    SDL_GLContext glcontext;
    ZsVulkan vkcontext;

    ZSGAME_DESC* desc;
public:
    ZSpireEngine(ZSENGINE_CREATE_INFO* info, ZSWINDOW_CREATE_INFO* win, ZSGAME_DESC* desc);

    void loadGame();
};

#endif // ENGINE_H
