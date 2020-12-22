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
#include "../../headers/engine/Resources.hpp"
#include <vector>
#include "../render/Framebuffer.hpp"
#include "../ui/Layouts/ILayout.hpp"

#define LIGHT_STRUCT_SIZE 64

enum class PIPELINE_STATE {
    PIPELINE_STATE_DEFAULT,
    PIPELINE_STATE_PICKING,
    PIPELINE_STATE_MARKED,
    PIPELINE_STATE_SHADOWDEPTH
};

namespace Engine{

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

class World;

class Renderer : public IEngineComponent {
protected:
    //Vector to store lights
    std::vector<void*> lights_ptr;
    RenderSettings render_settings;

    Framebuffer* gbuffer;
    Framebuffer* df_light_buffer;
    Framebuffer* ui_buffer;

    void setLightsToBuffer();
    void updateShadersCameraInfo(Engine::Camera* cam_ptr);
    
private:
    virtual void InitShaders(){}
public:
    PIPELINE_STATE current_state;

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
    Engine::Shader* water_shader;

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
    UniformBuffer* GetTerrainUniformBuffer() { return terrainUniformBuffer; }
    UniformBuffer* GetTransformUniformBuffer() { return transformBuffer; }

    void processObjects(World* world_ptr);

    void addLight(void* light_ptr);
    void removeLights();

    void TryRenderShadows(Engine::Camera* cam);
    void TryRenderSkybox();

    void renderUI();

    void renderSprite(Engine::Texture* texture_sprite, int X, int Y, int scaleX, int scaleY);
    void renderSprite(Engine::TextureResource* texture_sprite, int X, int Y, int scaleX, int scaleY);
    void renderGlyph(Engine::Texture* glyph, int X, int Y, int scaleX, int scaleY, ZSRGBCOLOR color);

    void render();
    virtual void render2D(){}
    virtual void render3D(Engine::Camera* cam){}
    virtual void DrawObject(Engine::GameObject* obj){}
    void renderShadowDepth(World* world_ptr, unsigned int CascadesNum);

    void OnCreate();
    virtual void initManager(){}

   
    void destroy();

    Engine::Camera* cam;

    Renderer();
    ~Renderer();
};

}
