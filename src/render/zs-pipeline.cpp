#include "../../headers/render/zs-pipeline.h"

void EngineRenderPipeline::initShaders(){
    this->tile_shader.compileFromFile("Shaders/2d_tile/tile2d.vs", "Shaders/2d/tile2d.fs");
    this->deffered_shader.compileFromFile("Shaders/postprocess/deffered_light/deffered.vs", "Shaders/postprocess/deffered_light/deffered.fs");
}

EngineRenderPipeline::EngineRenderPipeline(){

    initShaders();
}
