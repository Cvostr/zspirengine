#ifndef zs_GAME
#define zs_GAME

#include "world/World.hpp"
#include "render/Renderer.hpp"
#include "engine/Resources.hpp"
#include "misc/output.hpp"
#include "Scripting/AngelScriptMgr.h"
#include "ui/UiManager.hpp"
#include "audio/OALManager.hpp"
#include "vulkan/ZSVulkan.hpp"

typedef struct ZSGAME_DATA{
    //Pointer to resource manager
    Engine::ResourceManager* resources; 
    //Pointer to World class
    Engine::World* world;
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
    //TRUE, if this struct was created by editor
    bool isEditor; 

    ZSGAME_DATA() {
        resources = nullptr;
        world = nullptr;
        pipeline = nullptr;
        glyph_manager = nullptr;
        isEditor = false;
        script_manager = nullptr;
        out_manager = nullptr;
        ui_manager = nullptr;
        oal_manager = nullptr;
        vk_main = nullptr;
    }
}ZSGAME_DATA;

#endif
