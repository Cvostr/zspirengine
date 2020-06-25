#ifndef zs_GAME
#define zs_GAME

#include "world/World.h"
#include "render/zs-pipeline.h"
#include "engine/resources.h"

typedef struct ZSGAME_DATA{
    Engine::ResourceManager* resources; 
    //Pointer to World class
    Engine::World* world;
    Engine::RenderPipeline* pipeline;
    GlyphManager* glyph_manager;
    //TRUE, if this struct was created by editor
    bool isEditor; 

    ZSGAME_DATA() {
        resources = nullptr;
        world = nullptr;
        pipeline = nullptr;
        glyph_manager = nullptr;
        isEditor = false;
    }
}ZSGAME_DATA;

#endif
