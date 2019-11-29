#ifndef zs_pipeline
#define zs_pipeline

#include "zs-shader.h"
#include "zs-mesh.h"
#include "../../headers/engine/EngineManager.h"
#include "../../headers/render/zs-uniform-buffer.h"
#include "../../headers/ogl/ogl.h"
#include "../../headers/vulkan/vk_data.h"
#include "SDL2/SDL.h"
#include "GL/glew.h"
#include "../world/zs-camera.h"
#include <vector>

namespace Engine{

UniformBuffer* allocUniformBuffer();


enum GO_RENDER_TYPE {
    GO_RENDER_TYPE_NONE,
    GO_RENDER_TYPE_TILE,
    GO_RENDER_TYPE_MATERIAL
};

struct RenderSettings {
    ZSRGBCOLOR ambient_light_color;

    void defaults();

    RenderSettings(){
        defaults();
    }
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

class RenderPipeline : public EngineComponentManager{
private:
    bool depthTest; //if enabled, GL will do depth testing
    bool cullFaces; //if enabled, GL will cull faces

    G_BUFFER_GL gbuffer;

    RenderSettings render_settings;
    //Vector to store lights
    std::vector<void*> lights_ptr;

    void initShaders();
    void updateShadersCameraInfo(Engine::Camera* cam_ptr);
    void setLightsToBuffer();
public:
    Engine::Shader* tile_shader;
    Engine::Shader* deffered_shader;
    Engine::Shader* default3d;
    Engine::Shader* terrain_shader;

    UniformBuffer* transformBuffer;
    UniformBuffer* lightsBuffer;
    UniformBuffer* shadowBuffer;
    UniformBuffer* terrainUniformBuffer;
    UniformBuffer* skinningUniformBuffer;
    UniformBuffer* tileBuffer;
    UniformBuffer* skyboxTransformUniformBuffer;
    UniformBuffer* uiUniformBuffer;

    RenderSettings* getRenderSettings();
    Engine::Shader* getTileShader();

    void addLight(void* light_ptr);
    void removeLights();

    void render();
    void render2D();
    void render3D();
    void init();
    void destroy();

    RenderPipeline();
    ~RenderPipeline();
};

}

#endif
