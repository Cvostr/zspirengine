#ifndef zs_pipeline
#define zs_pipeline

#include "zs-shader.h"
#include "SDL2/SDL.h"
#include "GL/glew.h"

namespace Engine{

enum GO_RENDER_TYPE {
    GO_RENDER_TYPE_NONE,
    GO_RENDER_TYPE_TILE,
    GO_RENDER_TYPE_MATERIAL
};

class G_BUFFER_GL{
protected:
    unsigned int depthBuffer;
    unsigned int gBuffer; //framebuffer
    unsigned int tDiffuse; //To store RGB diffuse Color A - shininess
    unsigned int tNormal; //To store normal coordinate
    unsigned int tPos; //To store position coordinate
    unsigned int tTransparent; //To store color with alpha
public:
    G_BUFFER_GL();
    void create(int width, int height);
    void bindFramebuffer();
    void bindTextures();
    void Destroy();
};

class RenderPipeline{
private:
    G_BUFFER_GL gbuffer;

    Engine::Shader tile_shader;
    Engine::Shader deffered_shader;

    void initShaders();
public:
    void render();

    RenderPipeline();
};

}

#endif
