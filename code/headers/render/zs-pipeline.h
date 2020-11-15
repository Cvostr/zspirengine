#pragma once

#include "Shader.hpp"
#include "Mesh.hpp"
#include "../../headers/engine/EngineComponent.h"
#include "../../headers/render/UniformBuffer.hpp"
#include "../../headers/ogl/ogl.h"
#include "../../headers/vulkan/vk_data.h"
#include "SDL2/SDL.h"
#include "GL/glew.h"
#include "../world/Camera.hpp"
#include "../../headers/engine/resources.h"
#include <vector>
#include "zs-screen-effect.h"
#include "../ui/Layouts/ILayout.hpp"

enum class PIPELINE_STATE {
    PIPELINE_STATE_DEFAULT,
    PIPELINE_STATE_PICKING,
    PIPELINE_STATE_MARKED,
    PIPELINE_STATE_SHADOWDEPTH
};

namespace Engine{

UniformBuffer* allocUniformBuffer();

class GameObject;

struct RenderSettings {
    ZSRGBCOLOR ambient_light_color;

    GameObject* skybox_obj_ptr;
    GameObject* shadowcaster_obj_ptr;
        
    unsigned int mShadowCascadesNum;

    void defaults();
    void resetPointers();

    RenderSettings(){
        defaults();
    }
};

class LightsourceProperty;
class World;

class RenderPipeline : public IEngineComponent {
protected:
    //Vector to store lights
    std::vector<LightsourceProperty*> lights_ptr;
    RenderSettings render_settings;

    GLframebuffer* gbuffer;
    GLframebuffer* df_light_buffer;
    GLframebuffer* ui_buffer;

    void setLightsToBuffer();
    void updateShadersCameraInfo(Engine::Camera* cam_ptr);

    void setClearColor(float r, float g, float b, float a);
    void setBlendingState(bool blend);
    void setDepthState(bool depth);
    void setFaceCullState(bool face_cull);
    void setFullscreenViewport(unsigned int Width, unsigned int Height);
    void ClearFBufferGL(bool clearColor, bool clearDepth);

    void create_G_Buffer(unsigned int width, unsigned int height);
private:
    void initShaders();
public:
    PIPELINE_STATE current_state;

    ILayout* RootLayout;

    bool cullFaces; //if enabled, GL will cull faces
    bool allowOnUpdate;

    Engine::Shader* tile_shader;
    Engine::Shader* deffered_light;
    Engine::Shader* default3d;
    Engine::Shader* terrain_shader;
    Engine::Shader* skybox_shader;
    Engine::Shader* grass_shader;
    Engine::Shader* shadowMap;
    Engine::Shader* ui_shader;
    Engine::Shader* final_shader;

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
    Engine::Shader* getShadowmapShader();
    Engine::Shader* getUiShader();

    void processObjects(World* world_ptr);

    void addLight(LightsourceProperty* light_ptr);
    void removeLights();

    void TryRenderShadows(Engine::Camera* cam);
    void TryRenderSkybox();

    void renderUI();

    void renderSprite(Engine::Texture* texture_sprite, int X, int Y, int scaleX, int scaleY);
    void renderSprite(Engine::TextureResource* texture_sprite, int X, int Y, int scaleX, int scaleY);
    void renderGlyph(unsigned int texture_id, int X, int Y, int scaleX, int scaleY, ZSRGBCOLOR color);

    void render();
    void render2D();
    void render3D(Engine::Camera* cam);
    void renderShadowDepth(World* world_ptr, unsigned int CascadesNum);
    void init();
    void updateWindowSize(int W, int H);
    void destroy();

    Engine::Camera* cam;

    RenderPipeline();
    ~RenderPipeline();
};

}
