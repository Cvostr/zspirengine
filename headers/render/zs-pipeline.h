#ifndef zs_pipeline
#define zs_pipeline

#include "zs-shader.h"


enum GO_RENDER_TYPE {
    GO_RENDER_TYPE_NONE,
    GO_RENDER_TYPE_TILE,
    GO_RENDER_TYPE_MATERIAL
};

class EngineRenderPipeline{
    ZSPIRE::Shader tile_shader;
    ZSPIRE::Shader deffered_shader;

    void initShaders();
public:
    EngineRenderPipeline();
};



#endif
