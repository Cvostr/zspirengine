#include "../../headers/render/zs-pipeline.h"
#include "../../headers/engine.h"

extern ZSpireEngine* engine_ptr;

void EngineRenderPipeline::initShaders(){
    this->tile_shader.compileFromFile("Shaders/2d_tile/tile2d.vs", "Shaders/2d/tile2d.fs", engine_ptr);
    this->deffered_shader.compileFromFile("Shaders/postprocess/deffered_light/deffered.vs", "Shaders/postprocess/deffered_light/deffered.fs", engine_ptr);
}

EngineRenderPipeline::EngineRenderPipeline(){

    initShaders();
}

void EngineRenderPipeline::render(){

}
