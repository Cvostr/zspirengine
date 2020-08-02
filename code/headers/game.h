#ifndef zs_GAME
#define zs_GAME

#include "world/World.h"
#include "render/zs-pipeline.h"
#include "engine/resources.h"
#include "misc/output.hpp"
#include "Scripting/AngelScriptMgr.h"

typedef struct ZSGAME_DATA{
    //Pointer to resource manager
    Engine::ResourceManager* resources; 
    //Pointer to World class
    Engine::World* world;
    //Pointer to rendering pipeline class
    Engine::RenderPipeline* pipeline;
    //Pointer to glyph (font) manager
    GlyphManager* glyph_manager;
    Engine::AGScriptMgr* script_manager;
    Engine::OutputManager* out_manager;
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
    }
}ZSGAME_DATA;

#endif
