#ifndef zs_GAME
#define zs_GAME

#include "world/World.h"
#include "render/zs-pipeline.h"

typedef struct ZSGAME_DATA{

    Engine::World* world;
    EngineRenderPipeline* pipeline;

}ZSGAME_DATA;

#endif
