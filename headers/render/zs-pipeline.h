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

struct RenderSettings {
    ZSRGBCOLOR ambient_light_color;

    void* skybox_ptr;
    void* shadowcaster_ptr;

    void defaults();
    void resetPointers();

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
    unsigned int tMasks;
public:
    bool created;
    G_BUFFER_GL();
    void create(int width, int height);
    void bindFramebuffer();
    void bindTextures();
    void Destroy();
};

class RenderPipeline : public EngineComponentManager{
protected:
    //Vector to store lights
    std::vector<void*> lights_ptr;
    RenderSettings render_settings;
    G_BUFFER_GL gbuffer;

    void setLightsToBuffer();
    void updateShadersCameraInfo(Engine::Camera* cam_ptr);
private:

    void initShaders();


public:
    bool depthTest; //if enabled, GL will do depth testing
    bool cullFaces; //if enabled, GL will cull faces

    Engine::Shader* tile_shader;
    Engine::Shader* deffered_light;
    Engine::Shader* default3d;
    Engine::Shader* terrain_shader;
    Engine::Shader* skybox_shader;
    Engine::Shader* grass_shader;
    Engine::Shader* shadowMap;
    Engine::Shader* ui_shader;

    UniformBuffer* transformBuffer; //0
    UniformBuffer* lightsBuffer;    //1
    UniformBuffer* shadowBuffer;    //2
    UniformBuffer* terrainUniformBuffer; //3
    UniformBuffer* skinningUniformBuffer; //4
    UniformBuffer* tileBuffer;  //5
    UniformBuffer* skyboxTransformUniformBuffer; //6
    UniformBuffer* uiUniformBuffer; //7
    UniformBuffer* instancedTransformBuffer; //8

    RenderSettings* getRenderSettings();
    Engine::Shader* getTileShader();

    void addLight(void* light_ptr);
    void removeLights();

    void renderSprite(Engine::Texture* texture_sprite, int X, int Y, int scaleX, int scaleY);
    void renderGlyph(unsigned int texture_id, int X, int Y, int scaleX, int scaleY, ZSRGBCOLOR color);

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
