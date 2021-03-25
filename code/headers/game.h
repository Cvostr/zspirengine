#ifndef zs_GAME
#define zs_GAME

#include "world/World.hpp"
#include "world/WorldLoadManager.hpp"
#include "render/Renderer.hpp"
#include "engine/Resources.hpp"
#include "misc/output.hpp"
#include "Scripting/AngelScriptMgr.h"
#include "ui/UiManager.hpp"
#include "audio/OALManager.hpp"
#include "vulkan/ZSVulkan.hpp"
#include "engine/Time.hpp"
#include "engine/Window.hpp"

typedef struct ZSGAME_DATA{
    Engine::Window* window;
    //Pointer to resource manager
    Engine::ResourceManager* resources; 
    //Pointer to World class
    Engine::World* world;
    Engine::WorldLoadManager* wlm;
    //Pointer to rendering pipeline class
    Engine::Renderer* pipeline;
    //Pointer to glyph (font) manager
    GlyphManager* glyph_manager;
    Engine::AGScriptMgr* script_manager;
    Engine::OutputManager* out_manager;
    //Pointer to ui manager
    Engine::UiManager* ui_manager;
    Engine::OALManager* oal_manager;
    Engine::ZSVulkan* vk_main;
    Engine::Time* time;
    //TRUE, if this struct was created by editor
    bool isEditor; 

    ZSGAME_DATA() {
        resources = nullptr;
        world = nullptr;
        pipeline = nullptr;
        glyph_manager = new GlyphManager;
        isEditor = false;
        script_manager = nullptr;
        out_manager = new Engine::OutputManager;;
        ui_manager = new Engine::UiManager;
        oal_manager = new Engine::OALManager;
        vk_main = nullptr;
        time = new Engine::Time;

        wlm = nullptr;
        window = nullptr;
    }
}ZSGAME_DATA;

#endif
