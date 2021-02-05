#include "../../headers/ogl/GLRenderer.hpp"
#include "../../headers/game.h"

#include "../../headers/world/ObjectsComponents/MeshComponent.hpp"
#include "../../headers/world/ObjectsComponents/TerrainComponent.hpp"
#include "../../headers/world/ObjectsComponents/ShadowCasterComponent.hpp"
#include "../../headers/world/ObjectsComponents/CameraComponent.hpp"

extern ZSpireEngine* engine_ptr;
extern ZSGAME_DATA* game_data;

Engine::GLRenderer::GLRenderer() {
    InitShaders();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void Engine::GLRenderer::InitShaders() {
    

    this->mUiShader->compileFromFile("Shaders/ui/ui.vert", "Shaders/ui/ui.frag");
    this->mTextShader->compileFromFile("Shaders/ui/ui.vert", "Shaders/ui/text.frag");

    if (engine_ptr->desc->game_perspective == PERSP_2D) {
        this->tile_shader->compileFromFile("Shaders/2d_tile/tile2d.vert", "Shaders/2d_tile/tile2d.frag");
    }
    if (engine_ptr->desc->game_perspective == PERSP_3D) {

        this->deffered_light->compileFromFile("Shaders/postprocess/deffered_light/deffered.vert", "Shaders/postprocess/deffered_light/deffered.frag");
        this->default3d->compileFromFile("Shaders/3d/3d.vert", "Shaders/3d/3d.frag");
        this->mSkyboxShader->compileFromFile("Shaders/skybox/skybox.vert", "Shaders/skybox/skybox.frag");
        this->mTerrainShader->compileFromFile("Shaders/heightmap/heightmap.vert", "Shaders/heightmap/heightmap.frag");
        this->grass_shader->compileFromFile("Shaders/heightmap/grass.vert", "Shaders/heightmap/grass.frag");
        this->mShadowMapShader->compileFromFile("Shaders/shadowmap/shadowmap.vert", "", "Shaders/shadowmap/shadowmap.geom");
        this->final_shader->compileFromFile("Shaders/postprocess/final/final.vert", "Shaders/postprocess/final/final.frag");
        this->water_shader->compileFromFile("Shaders/water/water.vert", "Shaders/water/water.frag");

        MtShProps::genDefaultMtShGroup(default3d, mSkyboxShader, mTerrainShader, water_shader);
    }

    CopyEffect = new GLScreenEffect();
    CopyEffect->CompileShaderFromFile("Shaders/postprocess/copy/copy.comp");
    CopyEffect->SetLocalSize(8, 8);
}

void Engine::GLRenderer::create_G_Buffer_GL(unsigned int width, unsigned int height) {
    gbuffer = new GLframebuffer(width, height);
    gbuffer->Create();

    //effect = new GLScreenEffect(width, height, FORMAT_RGBA);
    //effect->CompileShaderFromFile("Shaders/postprocess/blur/blur.comp");
    ((GLframebuffer*)gbuffer)->AddDepth();
    ((GLframebuffer*)gbuffer)->AddTexture(FORMAT_RGBA); //Diffuse map
    ((GLframebuffer*)gbuffer)->AddTexture(FORMAT_RGB16F); //Normal map
    ((GLframebuffer*)gbuffer)->AddTexture(FORMAT_RGB16F); //Position map
    ((GLframebuffer*)gbuffer)->AddTexture(FORMAT_RGBA); //Transparent map
    ((GLframebuffer*)gbuffer)->AddTexture(FORMAT_RGBA); //Masks map

    df_light_buffer = new GLframebuffer(width, height);
    df_light_buffer->Create();
    ((GLframebuffer*)df_light_buffer)->AddTexture(FORMAT_RGBA); //Diffuse map
    ((GLframebuffer*)df_light_buffer)->AddTexture(FORMAT_RGBA); //Bloom map

    ui_buffer = new GLframebuffer(width, height);
    ui_buffer->Create();
    ((GLframebuffer*)ui_buffer)->AddTexture(FORMAT_RGBA); //UI Diffuse map

    //effect->PushInputTexture(((GLframebuffer*)df_light_buffer)->textures[1]);
}

void Engine::GLRenderer::initManager() {
    setDepthState(true);
    Engine::Window* win = engine_ptr->GetWindow();

    if (engine_ptr->desc->game_perspective == PERSP_2D) {
        cullFaces = false;
        setFaceCullState(false);
    }
    else {
        cullFaces = true;
        setFaceCullState(true);
    }
    //if we use opengl, then create GBUFFER in GL commands
    if (engine_ptr->desc->game_perspective == PERSP_3D) {
        create_G_Buffer_GL(win->GetWindowWidth(), win->GetWindowHeight());
    }
}


void Engine::GLRenderer::render2D() {
    World* world_ptr = game_data->world;
    Engine::Window* win = engine_ptr->GetWindow();

    setClearColor(0, 0, 0, 1);
    ClearFBufferGL(true, true);
    setBlendingState(true); //Disable blending to render Skybox and shadows
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    setFullscreenViewport(win->GetWindowWidth(), win->GetWindowHeight());
    setDepthState(true);
    //Render objects
    processObjects(world_ptr);
}

void Engine::GLRenderer::render3D(Engine::Camera* cam) {
    World* world_ptr = game_data->world;
    Engine::Window* win = engine_ptr->GetWindow();

    //render cameras
    for (unsigned int cam_i = 0; cam_i < mCameras.size(); cam_i++) {
        Render3DCamera(mCameras[cam_i]);
    }

    setFrontFace(CCF_DIRECTION_CCW);
    updateShadersCameraInfo(cam);

    //Render shadows, first
    TryRenderShadows(cam);

    render_settings.CurrentViewMask = 0xFFFFFFFFFFFFFFFF;
    this->mMainCamera = cam;
    {
        //Bind Geometry Buffer to make Deferred Shading
        ((GLframebuffer*)gbuffer)->bind();
        setClearColor(0, 0, 0, 0);
        ClearFBufferGL(true, true);
        setFullscreenViewport(win->GetWindowWidth(), win->GetWindowHeight());
        {
            //Render Skybox
            setDepthState(false);
            setBlendingState(false);
            setFaceCullState(false);
            TryRenderSkybox();
        }
        glEnablei(GL_BLEND, 0);
        glBlendFunci(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, 0);
        {
            //Render World
            setDepthState(true);
            setFaceCullState(true);
            //Render whole world
            processObjects(world_ptr);
        }
    }

    //Process Deffered lights
    {
        ((GLframebuffer*)df_light_buffer)->bind();
        ClearFBufferGL(true, false); //Clear screen
        //Disable depth rendering to draw plane correctly
        setDepthState(false);
        setFaceCullState(false);
        ((GLframebuffer*)gbuffer)->bindTextures(10); //Bind gBuffer textures
        deffered_light->Use(); //use deffered shader
        Engine::getPlaneMesh2D()->Draw(); //Draw screen
    }

    //effect->Compute();

    //Render ALL UI
    {
        ((GLframebuffer*)ui_buffer)->bind();
        setClearColor(0, 0, 0, 0);
        ClearFBufferGL(true, false); //Clear screen 
        setDepthState(false);
        setBlendingState(true);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        renderUI();
    }

    //Draw result into main buffer
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        ((GLframebuffer*)df_light_buffer)->bindTextures(0);
        ((GLframebuffer*)ui_buffer)->bindTextures(1);
        final_shader->Use();
        Engine::getPlaneMesh2D()->Draw(); //Draw screen
    }
}

void Engine::GLRenderer::Render3DCamera(void* cam_prop) {
    CameraComponent* cc = (CameraComponent*)(cam_prop);


    setFrontFace(cc->mCullFaceDirection);

    if (!cc->isActive())
        return;

    Camera* _cam = (Camera*)cc;

    cc->UpdateTextureResource();
    Texture* Texture = cc->mTarget->texture_ptr;

    if(cc->mAutoViewport)
        _cam->setViewport(Texture->GetWidth(), Texture->GetHeight());

    ZSVIEWPORT vp = _cam->getViewport();

    //glViewport(0, 0, vp.endX, vp.endY);

    render_settings.CurrentViewMask = cc->mViewMask;

    updateShadersCameraInfo(_cam);
    World* world_ptr = game_data->world;
    Engine::Window* win = engine_ptr->GetWindow();

    {
        //Bind Geometry Buffer to make Deferred Shading
        ((GLframebuffer*)gbuffer)->bind();
        setClearColor(0, 0, 0, 0);
        ClearFBufferGL(true, true);
        setFullscreenViewport(win->GetWindowWidth(), win->GetWindowHeight());
        {
            //Render Skybox
            setDepthState(false);
            setBlendingState(false);
            setFaceCullState(false);
            TryRenderSkybox();
        }
        glEnablei(GL_BLEND, 0);
        glBlendFunci(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, 0);
        {
            //Render World
            setDepthState(true);
            setFaceCullState(true);
            //Render whole world
            processObjects(world_ptr);
        }
    }

    //Process Deffered lights
    {
        ((GLframebuffer*)df_light_buffer)->bind();
        ClearFBufferGL(true, false); //Clear screen
        //Disable depth rendering to draw plane correctly
        setDepthState(false);
        setFaceCullState(false);
        ((GLframebuffer*)gbuffer)->bindTextures(10); //Bind gBuffer textures
        deffered_light->Use(); //use deffered shader
        Engine::getPlaneMesh2D()->Draw(); //Draw screen
    }

    //glFrontFace(GL_CCW);
    
    CopyEffect->ClearInputTextures();
    CopyEffect->PushInputTexture(df_light_buffer->GetTexture(0));
    CopyEffect->SetOutputTexture(Texture);
    CopyEffect->SetSize(Texture->GetWidth(), Texture->GetHeight());
    CopyEffect->Compute();
   }

void Engine::GLRenderer::DrawObject(Engine::GameObject* obj) {
    BoundingBox3 bb = obj->getBoundingBox();
    

    if (current_state == PIPELINE_STATE::PIPELINE_STATE_DEFAULT)
        //Call prerender on each property in object
        obj->onPreRender(this);
    if (obj->hasMesh() || obj->hasTerrain()) {
        //Send mesh skinning matrices
        if (obj->getBonesCount()) {
            obj->setSkinningMatrices(this);
            //Send bones
            skinningUniformBuffer->bind();
            skinningUniformBuffer->updateBufferedData();
        }
        //Send transform matrix to transform buffer
        Engine::TransformProperty* transform_ptr = obj->getTransformProperty();
        transformBuffer->bind();
        transformBuffer->writeData(sizeof(Mat4) * 2, sizeof(Mat4), &transform_ptr->transform_mat);

        if (current_state == PIPELINE_STATE::PIPELINE_STATE_DEFAULT) {
            MeshProperty* mesh_prop = obj->getPropertyPtr<MeshProperty>();
            //Render all properties
            obj->onRender(this);
            if (!obj->hasMaterial())
                game_data->resources->getMaterialByLabel("@default")->material->applyMatToPipeline();
            //Draw mesh
            obj->DrawMesh(this);
        }

        if (current_state == PIPELINE_STATE::PIPELINE_STATE_SHADOWDEPTH) {
            MeshProperty* mesh_prop = obj->getPropertyPtr<MeshProperty>();
            //Get castShadows boolean from several properties
            bool castShadows = (obj->hasTerrain()) ? obj->getPropertyPtr<TerrainProperty>()->castShadows : mesh_prop->castShadows;
            //If castShadows is true, then render mesh to DepthBuffer

            ShadowCasterProperty* caster = getRenderSettings()->shadowcaster_obj_ptr->getPropertyPtr<ShadowCasterProperty>();

            unsigned int InstNum = caster->mCascadesNum;

            if (bb.GetLongestDistance(mMainCamera->getCameraPosition()) < 20)
                InstNum = 1;

            if (castShadows)
                obj->DrawMeshInstanced(this, InstNum);
        }
    }
}

void Engine::GLRenderer::OnUpdateWindowSize(int W, int H) {
    setFullscreenViewport(W, H);

    if (engine_ptr->desc->game_perspective == PERSP_3D) {
        gbuffer->SetSize(W, H);
        df_light_buffer->SetSize(W, H);
        ui_buffer->SetSize(W, H);
    }
}

void Engine::GLRenderer::setFullscreenViewport(unsigned int Width, unsigned int Height) {
    glViewport(0, 0, Width, Height);
}

void Engine::GLRenderer::ClearFBufferGL(bool clearColor, bool clearDepth) {
    GLbitfield clearMask = 0;

    if (clearColor)
        clearMask |= GL_COLOR_BUFFER_BIT;
    if (clearDepth)
        clearMask |= GL_DEPTH_BUFFER_BIT;

    glClear(clearMask);
}

void Engine::GLRenderer::setClearColor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

void Engine::GLRenderer::setBlendingState(bool blend) {
    if (blend)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
}
void Engine::GLRenderer::setDepthState(bool depth) {
    if (depth)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}
void Engine::GLRenderer::setFaceCullState(bool face_cull) {
    if (face_cull)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);
}

void Engine::GLRenderer::setFrontFace(CameraCullFaceDirection CCFD) {

    GLenum mode = GL_CCW;

    if (CCFD == CCF_DIRECTION_CW)
        mode = GL_CW;

    glFrontFace(mode);
}