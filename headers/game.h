#ifndef zs_GAME
#define zs_GAME

#include "world/World.h"
#include "render/zs-pipeline.h"
#include "engine/resources.h"

typedef struct ZSGAME_DATA{
    Engine::ResourceManager* resources;
    Engine::World* world;
    Engine::RenderPipeline* pipeline;
    GlyphManager* glyph_manager;

}ZSGAME_DATA;

#endif
