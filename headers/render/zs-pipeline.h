#ifndef zs_pipeline
#define zs_pipeline

#include "zs-shader.h"

class EngineRenderPipeline{
    ZSPIRE::Shader tile_shader;
    ZSPIRE::Shader deffered_shader;

    void initShaders();
public:
    EngineRenderPipeline();
};

#endif
