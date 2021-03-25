#pragma once

#include "Shader.hpp"
#include "Mesh.hpp"
#include <engine/EngineComponent.h>
#include <render/UniformBuffer.hpp>
#include "SDL2/SDL.h"
#include <world/Camera.hpp>
#include <engine/Resources.hpp>
#include <vector>
#include <render/Framebuffer.hpp>
#include <ui/Layouts/ILayout.hpp>

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
    RGBAColor ambient_light_color;

    unsigned long long CurrentViewMask;

    GameObject* skybox_obj_ptr;
    GameObject* shadowcaster_obj_ptr;

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
    std::vector<void*> mLights;
    std::vector<void*> mCameras;
    std::vector<void*> mWinds;
    std::vector<void*> mParticleSystems;

    RenderSettings render_settings;


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
    Engine::Shader* mTerrainShader;
    Engine::Shader* mSkyboxShader;
    Engine::Shader* grass_shader;
    Engine::Shader* mShadowMapShader;
    Engine::Shader* mUiShader;
    Engine::Shader* mTextShader;
    Engine::Shader* final_shader;
    Engine::Shader* water_shader;
    Engine::Shader* default_particle;

    UniformBuffer* transformBuffer; //0
    UniformBuffer* lightsBuffer;    //1
    UniformBuffer* shadowBuffer;    //2
    UniformBuffer* MaterialsUniformBuffer; //3
    UniformBuffer* skinningUniformBuffer; //4
    UniformBuffer* skyboxTransformUniformBuffer; //6
    UniformBuffer* uiUniformBuffer; //7
    UniformBuffer* instancedTransformBuffer; //9
    UniformBuffer* TextUniformBuffer; //10
    UniformBuffer* TimeUniformBuffer; //11
    UniformBuffer* WindUniformBuffer; //12

    RenderSettings* getRenderSettings() { return &this->render_settings; }
    Engine::Shader* getTileShader() { return tile_shader; }
    Engine::Shader* getShadowmapShader() { return mShadowMapShader;  }
    Engine::Shader* getUiShader() { return mUiShader; }
    UniformBuffer* GetMaterialsUniformBuffer() { return MaterialsUniformBuffer; }
    UniformBuffer* GetTransformUniformBuffer() { return transformBuffer; }
    UniformBuffer* GetShadowmapUniformBuffer() { return shadowBuffer; }
    UniformBuffer* GetInstancedUniformBuffer() { return instancedTransformBuffer; }

    void processObjects(World* world_ptr);

    void lookForCameras(World* world_ptr);

    void TryRenderShadows(Engine::Camera* cam);
    void TryRenderSkybox();

    void renderUI();

    void renderSprite(Engine::Texture* texture_sprite, int X, int Y, int scaleX, int scaleY, RGBAColor color = RGBAColor(255, 255, 255));
    void renderSprite(Engine::TextureResource* texture_sprite, int X, int Y, int scaleX, int scaleY);
    void renderGlyph(CharacterGlyph* glyph, int X, int Y, int scaleX, int scaleY, RGBAColor color);

    void render();
    virtual void render2D(){}
    virtual void render3D(){}
    virtual void DrawObject(Engine::GameObject* obj){}
    virtual void DrawParticleSystem(Engine::GameObject* obj){}
    void renderShadowDepth(World* world_ptr, unsigned int CascadesNum);

    void OnCreate();
    virtual void initManager(){}

   
    void destroy();

    Engine::Camera* mMainCamera;
    void* mMainCameraComponent;

    explicit Renderer();
    ~Renderer();
};

}
