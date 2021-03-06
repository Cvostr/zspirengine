#include <render/Renderer.hpp>
#include <engine.h>
#include <game.h>
#include <world/tile_properties.h>
#include <world/ObjectsComponents/LightSourceComponent.hpp>
#include <world/ObjectsComponents/ShadowCasterComponent.hpp>
#include <world/ObjectsComponents/SkyboxComponent.hpp>
#include <world/ObjectsComponents/TerrainComponent.hpp>
#include <world/ObjectsComponents/MaterialComponent.hpp>
#include <world/ObjectsComponents/MeshComponent.hpp>
#include <world/ObjectsComponents/NodeComponent.hpp>
#include <world/ObjectsComponents/CameraComponent.hpp>
#include <world/ObjectsComponents/WindZoneComponent.hpp>
#include <world/ObjectsComponents/ParticleEmitterComponent.hpp>

extern ZSpireEngine* engine_ptr;
//Hack to support resources
extern ZSGAME_DATA* game_data;

void Engine::RenderSettings::defaults(){
    ambient_light_color = RGBAColor(255, 255, 255, 255);
    resetPointers();
}

void Engine::RenderSettings::resetPointers(){
    this->skybox_obj_ptr = nullptr;
    this->shadowcaster_obj_ptr = nullptr;
}

Engine::Renderer::Renderer(){
    mMainCameraComponent = nullptr;

    this->current_state = PIPELINE_STATE::PIPELINE_STATE_DEFAULT;
    this->cullFaces = false;

    this->mUiShader = Engine::allocShader();
    this->mTextShader = allocShader();

    if (engine_ptr->desc->game_perspective == PERSP_2D) {
        this->tile_shader = allocShader();
    }
    if (engine_ptr->desc->game_perspective == PERSP_3D) {
        this->default3d = allocShader();
        this->deffered_light = allocShader();
        this->mSkyboxShader = allocShader();
        this->mTerrainShader = allocShader();
        this->grass_shader = allocShader();
        this->mShadowMapShader = allocShader();
        this->final_shader = allocShader();
        this->water_shader = allocShader();
        this->default_particle = allocShader();
    }

    //Allocate transform buffer
    this->transformBuffer = allocUniformBuffer();
    transformBuffer->init(0, sizeof(Mat4) * 3 + 16);
    //allocate lights buffer
    lightsBuffer = allocUniformBuffer();
    lightsBuffer->init(1, LIGHT_STRUCT_SIZE * MAX_LIGHTS_AMOUNT + 16 * 2);
    //Shadow uniform buffer
    shadowBuffer = allocUniformBuffer();
    shadowBuffer->init(2, 448);
    //Terrain uniform buffer
    MaterialsUniformBuffer = allocUniformBuffer();
    MaterialsUniformBuffer->init(3, 2048);
    {
        //Skinning uniform buffer
        skinningUniformBuffer = allocUniformBuffer();
        skinningUniformBuffer->init(4, sizeof(Mat4) * MAX_MESH_BONES);
        for (unsigned int i = 0; i < MAX_MESH_BONES; i++) {
            Mat4 m = getIdentity();
            skinningUniformBuffer->writeData(sizeof(Mat4) * i, sizeof(Mat4), &m);
        }
    }

    //Skybox uniform buffer
    skyboxTransformUniformBuffer = Engine::allocUniformBuffer();
    skyboxTransformUniformBuffer->init(6, sizeof(Mat4) * 2);

    uiUniformBuffer = Engine::allocUniformBuffer();
    uiUniformBuffer->init(7, sizeof(Mat4) * 2 + 16);

    TextUniformBuffer = Engine::allocUniformBuffer();
    TextUniformBuffer->init(10, 16 * 2);

    TimeUniformBuffer = Engine::allocUniformBuffer();
    TimeUniformBuffer->init(11, sizeof(int) * 2);

    WindUniformBuffer = Engine::allocUniformBuffer();
    WindUniformBuffer->init(12, 32 * 100 + 4);

    {
        instancedTransformBuffer = Engine::allocUniformBuffer();
        instancedTransformBuffer->init(9, sizeof(Mat4) * INSTANCED_RENDER_BUFFER_SIZE);

        for (unsigned int i = 0; i < INSTANCED_RENDER_BUFFER_SIZE; i++) {
            Mat4 m = getIdentity();
            instancedTransformBuffer->writeData(sizeof(Mat4) * i, sizeof(Mat4), &m);
        }
    }


    Engine::setupDefaultMeshes();
    //to avoid memory reallocations
    this->mLights.reserve(MAX_LIGHTS_AMOUNT);

    allowOnUpdate = true;
}

Engine::Renderer::~Renderer(){
    destroy();
}

void Engine::Renderer::OnCreate(){
    initManager();
}

void Engine::Renderer::destroy(){
    if(engine_ptr->desc->game_perspective == PERSP_2D){
        delete tile_shader;
    }

    delete mUiShader;

    delete lightsBuffer;
    delete transformBuffer;
    delete MaterialsUniformBuffer;
    delete skinningUniformBuffer;
    delete skyboxTransformUniformBuffer;
    delete instancedTransformBuffer;
    delete TextUniformBuffer;
    delete TimeUniformBuffer;

    if(engine_ptr->desc->game_perspective == PERSP_3D){
        delete deffered_light;
        delete default3d;
        delete mSkyboxShader;
        delete mTerrainShader;
        delete grass_shader;
        delete mShadowMapShader;
        delete water_shader;


    }

    MtShProps::clearMtShaderGroups();
}

void Engine::Renderer::setLightsToBuffer(){
    //Bind Lighting buffer to work with it
    this->lightsBuffer->bind();
    //Iterate over all lights
    for(unsigned int light_i = 0; light_i < this->mLights.size(); light_i ++){
        LightsourceComponent* _light_ptr = static_cast<LightsourceComponent*>(mLights[light_i]);

        LIGHTSOURCE_TYPE light_type = (_light_ptr->light_type);

        lightsBuffer->writeDataBuffered(LIGHT_STRUCT_SIZE * light_i, sizeof (LIGHTSOURCE_TYPE), &light_type);
        if(_light_ptr->light_type > LIGHTSOURCE_TYPE::LIGHTSOURCE_TYPE_DIRECTIONAL){
            lightsBuffer->writeDataBuffered(LIGHT_STRUCT_SIZE * light_i + 4, sizeof (float), &_light_ptr->range);
            lightsBuffer->writeDataBuffered(LIGHT_STRUCT_SIZE * light_i + 12, sizeof (float), &_light_ptr->spot_angle);
        }
        lightsBuffer->writeDataBuffered(LIGHT_STRUCT_SIZE * light_i + 8, sizeof (float), &_light_ptr->intensity);

        lightsBuffer->writeDataBuffered(LIGHT_STRUCT_SIZE * light_i + 16, 12, &_light_ptr->last_pos);
        lightsBuffer->writeDataBuffered(LIGHT_STRUCT_SIZE * light_i + 32, 12, &_light_ptr->direction);
        lightsBuffer->writeDataBuffered(LIGHT_STRUCT_SIZE * light_i + 48, sizeof (int), &_light_ptr->color.gl_r);
        lightsBuffer->writeDataBuffered(LIGHT_STRUCT_SIZE * light_i + 52, sizeof (int), &_light_ptr->color.gl_g);
        lightsBuffer->writeDataBuffered(LIGHT_STRUCT_SIZE * light_i + 56, sizeof (int), &_light_ptr->color.gl_b);
    }

    int ls = static_cast<int>(mLights.size());
    lightsBuffer->writeDataBuffered(LIGHT_STRUCT_SIZE * MAX_LIGHTS_AMOUNT, 4, &ls);

    Vec3 ambient_L = Vec3(render_settings.ambient_light_color.r / 255.0f,render_settings.ambient_light_color.g / 255.0f, render_settings.ambient_light_color.b / 255.0f);
    lightsBuffer->writeDataBuffered(LIGHT_STRUCT_SIZE * MAX_LIGHTS_AMOUNT + 16, 12, &ambient_L);

    lightsBuffer->updateBufferedData();
}

void Engine::Renderer::render(){
    World* world_ptr = game_data->world;
    lookForCameras(world_ptr);
    //send lights to uniform buffer
    setLightsToBuffer();

    switch(engine_ptr->desc->game_perspective){
        case PERSP_2D:{
            render2D();
            break;
        }
        case PERSP_3D :{
            render3D();
            break;
        }
    }
}


void Engine::Renderer::processObjects(World* world_ptr) {
    World* _world_ptr = static_cast<World*>(world_ptr);
    for (unsigned int obj_i = 0; obj_i < _world_ptr->objects.size(); obj_i++) {
        GameObject* obj_ptr = _world_ptr->objects[obj_i];
        if (!obj_ptr->hasParent) //if it is a root object
            obj_ptr->processObject(this); //Draw object
    }
}

void Engine::Renderer::renderShadowDepth(World* world_ptr, unsigned int CascadesNum){
    World* _world_ptr = static_cast<World*>(world_ptr);
    current_state = PIPELINE_STATE::PIPELINE_STATE_SHADOWDEPTH;
    //Iterate over all objects in the world
    processObjects(world_ptr);
    current_state = PIPELINE_STATE::PIPELINE_STATE_DEFAULT;
}

void Engine::GameObject::processObject(Renderer* pipeline) {
    if (mAlive == false || mActive == false) return;

    Engine::TransformProperty* transform_prop = this->getTransformProperty();

    if(pipeline->getRenderSettings()->CurrentViewMask & this->mViewMask)
        pipeline->DrawObject(this);

    for (unsigned int obj_i = 0; obj_i < this->mChildren.size(); obj_i++) {
        if (!mChildren[obj_i].isEmpty()) { //if link isn't broken
            mChildren[obj_i].updLinkPtr();
            GameObject* child_ptr = this->mChildren[obj_i].ptr;
            if (child_ptr == nullptr) return;
            child_ptr->processObject(pipeline);
        }
    }
}

void Engine::GameObject::setSkinningMatrices(Renderer* pipeline) {
    MeshProperty* mesh_prop = getPropertyPtr<MeshProperty>();
    //Send all skinning matrices
    if (hasMesh()) {
        //Iterate over all bones
        for (unsigned int bone_i = 0; bone_i < getBonesCount(); bone_i++) {
            //Check for buffer overflow
            if (bone_i >= MAX_MESH_BONES)
                break;
            //Obtain bone by pointer
            Engine::Bone* b = &mesh_prop->mesh_ptr->mesh_ptr->bones[bone_i];

            Engine::GameObject* node = nullptr;
            GameObject* RootNode = mesh_prop->skinning_root_node;
            Mat4 rootNodeTransform;

            if (RootNode != nullptr) {
                NodeProperty* RootNodeProp = RootNode->getPropertyPtr<Engine::NodeProperty>();
                if (RootNodeProp != nullptr) {
                    //if RootNode is specified
                    node = (mesh_prop->skinning_root_node)->getChildObjectWithNodeLabel(b->bone_name);
                    //Get root transform
                    rootNodeTransform = (RootNodeProp->transform_mat);
                }
            }

            if (node != nullptr) {
                Engine::NodeProperty* nd = node->getPropertyPtr<Engine::NodeProperty>();
                //Calculate result matrix
                Mat4 matrix = transpose(invert(rootNodeTransform) * nd->abs * b->offset);
                //Send skinned matrix to skinning uniform buffer
                
                pipeline->skinningUniformBuffer->writeDataBuffered(sizeof(Mat4) * bone_i, sizeof(Mat4), &matrix);
            }
        }
    }
}

void Engine::MaterialProperty::onRender(Engine::Renderer* pipeline){
    //Check for validity of pointer
    if(mMaterial == nullptr) return;

    MaterialTemplate* group_ptr = mMaterial->mTemplate;
    if( group_ptr == nullptr) return ; //if object hasn't property

    //Get pointer to shadowcaster
    Engine::ShadowCasterProperty* shadowcast = nullptr;
    if (pipeline->getRenderSettings()->shadowcaster_obj_ptr != nullptr) {
        shadowcast =
            pipeline->getRenderSettings()->shadowcaster_obj_ptr->getPropertyPtr<Engine::ShadowCasterProperty>();
        
        int recShadows = 1;

        if(shadowcast == nullptr){
            //In GLSL we should use Integer instead of bool
            recShadows = 0;
        }else if(!mReceiveShadows || !shadowcast->isActive()){
            recShadows = 0;
        }else
            shadowcast->setTexture();

        //Bind shadow uniform buffer
        pipeline->shadowBuffer->bind();
        pipeline->shadowBuffer->writeDataBuffered(8, sizeof(int), &recShadows);
        pipeline->shadowBuffer->updateBufferedData();
    }
    //Apply matrerial to shader and textures
    mMaterial->applyMatToPipeline();
}

void Engine::SkyboxProperty::onPreRender(Engine::Renderer* pipeline){
    pipeline->getRenderSettings()->skybox_obj_ptr = this->go_link.updLinkPtr();
}

void Engine::SkyboxProperty::DrawSky(Renderer* pipeline){
    if(!this->isActive())
        return;
    if(this->go_link.updLinkPtr() == nullptr) return;
    //Get pointer to Material property
    MaterialProperty* mat = this->go_link.updLinkPtr()->getPropertyPtr<MaterialProperty>();
    if(mat == nullptr) return;
    //Apply material shader
    mat->onRender(pipeline);
    //Draw skybox cube
    Engine::getSkyboxMesh()->Draw();
}

void Engine::TileProperty::onRender(Engine::Renderer* pipeline){
    pipeline->GetMaterialsUniformBuffer()->bind();

    //Checking for diffuse texture
    if(texture_diffuse != nullptr){
        texture_diffuse->Use(0); //Use this texture
    }

    int diffuse1_ = texture_diffuse != nullptr;
    pipeline->GetMaterialsUniformBuffer()->writeData(20, 4, &diffuse1_);

    //Checking for transparent texture
    if(texture_transparent != nullptr){
        texture_transparent->Use(1); //Use this texture
    }
    int diffuse2_ = texture_transparent != nullptr;
    pipeline->GetMaterialsUniformBuffer()->writeData(24, 4, &diffuse2_);
    //calculate animation state
    int anim_state_i = anim_state.playing;
    //Sending animation info
    if(anim_property.isAnimated && anim_state.playing == true){ //If tile animated, then send anim state to shader
        pipeline->GetMaterialsUniformBuffer()->writeData(16, 4, &anim_state_i);
        pipeline->GetMaterialsUniformBuffer()->writeData(0, 4, &anim_property.framesX);
        pipeline->GetMaterialsUniformBuffer()->writeData(4, 4, &anim_property.framesY);
        pipeline->GetMaterialsUniformBuffer()->writeData(8, 4, &anim_state.cur_frameX);
        pipeline->GetMaterialsUniformBuffer()->writeData(12, 4, &anim_state.cur_frameY);
    }else{ //No animation or unplayed
        pipeline->GetMaterialsUniformBuffer()->writeData(16, 4, &anim_state_i);
    }
}

void Engine::Renderer::updateShadersCameraInfo(Engine::Camera* cam_ptr){
    transformBuffer->bind();
    Mat4 proj = cam_ptr->getProjMatrix();
    Mat4 view = cam_ptr->getViewMatrix();
    Vec3 cam_pos = cam_ptr->getCameraPosition();
    transformBuffer->writeData(0, sizeof (Mat4), &proj);
    transformBuffer->writeData(sizeof (Mat4), sizeof (Mat4), &view);
    transformBuffer->writeData(sizeof (Mat4) * 3, sizeof(Vec3), &cam_pos);
    //Setting UI camera to UI buffer

    uint32_t Width = engine_ptr->GetWindow()->GetWindowWidth();
    uint32_t Height = engine_ptr->GetWindow()->GetWindowHeight();
    Mat4 UImatrix = getOrthogonal(
        0, 
        static_cast<float>(Width), 
        0,
        static_cast<float>(Height));
    uiUniformBuffer->bind();
    uiUniformBuffer->writeData(0, sizeof (Mat4), &UImatrix);
    
    //Setting cameras to skybox shader
    proj = cam_ptr->getProjMatrix();
    view = removeTranslationFromViewMat(view);
    skyboxTransformUniformBuffer->bind();
    skyboxTransformUniformBuffer->writeData(0, sizeof (Mat4), &proj);
    skyboxTransformUniformBuffer->writeData(sizeof (Mat4), sizeof (Mat4), &view);

    uint32_t Frames = game_data->time->GetFramesCount();
    float DeltaTime = game_data->time->GetDeltaTime();
    TimeUniformBuffer->bind();
    TimeUniformBuffer->writeData(0, sizeof(int), &Frames);
    TimeUniformBuffer->writeData(sizeof(int), sizeof(float), &DeltaTime);
}

void Engine::Renderer::renderUI() {
    game_data->ui_manager->DrawRootLayout();
}

void Engine::Renderer::renderSprite(Engine::Texture* texture_sprite, int X, int Y, int scaleX, int scaleY, RGBAColor color){
    this->mUiShader->Use();
    uiUniformBuffer->bind();

    //Use texture at 0 slot
    texture_sprite->Use(0);

    Mat4 translation = getTranslationMat(static_cast<float>(X), static_cast<float>(Y), 0.0f);
    Mat4 scale = getScaleMat(static_cast<float>(scaleX), static_cast<float>(scaleY), 0.0f);
    Mat4 transform = scale * translation;

    //Push glyph transform
    uiUniformBuffer->writeData(sizeof (Mat4), sizeof (Mat4), &transform);
    //sending glyph color
    uiUniformBuffer->writeData(sizeof(Mat4) * 2, 4, &color.gl_r);
    uiUniformBuffer->writeData(sizeof(Mat4) * 2 + 4, 4, &color.gl_g);
    uiUniformBuffer->writeData(sizeof(Mat4) * 2 + 8, 4, &color.gl_b);

    Engine::getUiSpriteMesh2D()->Draw();
}

void Engine::Renderer::renderSprite(Engine::TextureResource* texture_sprite, int X, int Y, int scaleX, int scaleY){
    if (texture_sprite != nullptr) {
        texture_sprite->Use(0);
        renderSprite(texture_sprite->texture_ptr, X, Y, scaleX, scaleY);
    }
}

void Engine::Renderer::renderGlyph(CharacterGlyph* glyph, int X, int Y, int scaleX, int scaleY, RGBAColor color){
    this->mTextShader->Use();
    TextUniformBuffer->bind();
    //sending glyph color
    TextUniformBuffer->writeData(0, 4, &color.gl_r);
    TextUniformBuffer->writeData(4, 4, &color.gl_g);
    TextUniformBuffer->writeData(8, 4, &color.gl_b);
    
    Vec2 uv_start = glyph->mGlyphTextureStart;
    Vec2 uv_end = glyph->mGlyphSize;

    uv_start /= 2048.f;
    uv_end /= 2048.f;

    TextUniformBuffer->writeData(16, 4, &uv_start.X);
    TextUniformBuffer->writeData(16 + 4, 4, &uv_start.Y);

    TextUniformBuffer->writeData(16 + 8, 4, &uv_end.X);
    TextUniformBuffer->writeData(16 + 12, 4, &uv_end.Y);

    Mat4 translation = getTranslationMat(static_cast<float>(X), static_cast<float>(Y), 0.0f);
    Mat4 scale = getScaleMat(static_cast<float>(scaleX), static_cast<float>(scaleY), 0.0f);
    Mat4 transform = scale * translation;

    //Push glyph transform
    uiUniformBuffer->bind();
    uiUniformBuffer->writeData(sizeof (Mat4), sizeof (Mat4), &transform);

    Engine::getUiSpriteMesh2D()->Draw();
}

void Engine::Renderer::lookForCameras(World* world_ptr) {
    mCameras.clear();
    mLights.clear();
    mWinds.clear();
    mParticleSystems.clear();

    mMainCameraComponent = nullptr;
    mMainCamera = nullptr;
    for (size_t object_i = 0; object_i < world_ptr->objects.size(); object_i++) {
        GameObject* object = world_ptr->objects[object_i];
        if (object->mAlive && object->isActive()) {
            //Call update on every property in objects
            if (allowOnUpdate)
                object->onUpdate(game_data->time->GetDeltaTime());

            CameraComponent* cam = object->getPropertyPtr<CameraComponent>();
            LightsourceComponent* light = object->getPropertyPtr<LightsourceComponent>();
            WindZoneComponent* wind = object->getPropertyPtr<WindZoneComponent>();
            ParticleEmitterComponent* emitter = object->getPropertyPtr<ParticleEmitterComponent>();

            if (cam != nullptr) {
                if (cam->isActive()) {
                    cam->UpdateFramebuffers();
                    mCameras.push_back(cam);
                    if (cam->mIsMainCamera) {
                        mMainCameraComponent = cam;
                        mMainCamera = (Camera*)cam;
                    }
                    
                }
            }

            if (light != nullptr) {
                if (light->isActive()) {
                    mLights.push_back(light);
                }
            }

            if (wind != nullptr) {
                if (wind->isActive()) {
                    mWinds.push_back(wind);
                }
            }

            if (emitter != nullptr) {
                if (emitter->isActive()) {

                    mParticleSystems.push_back(emitter);

                    emitter->StepSimulation();

                }
            }
        }
    }
}

void Engine::Renderer::TryRenderShadows(Engine::Camera* cam) {
    if (this->render_settings.shadowcaster_obj_ptr != nullptr) {
        Engine::ShadowCasterProperty* shadowcast =
            static_cast<Engine::GameObject*>(this->render_settings.shadowcaster_obj_ptr)->getPropertyPtr<Engine::ShadowCasterProperty>();
        if (shadowcast != nullptr) { //we have shadowcaster
            shadowcast->Draw(cam, this); //draw shadowcaster
        }
    }
}
void Engine::Renderer::TryRenderSkybox() {
    if (this->render_settings.skybox_obj_ptr != nullptr) {
        Engine::SkyboxProperty* skybox
            = static_cast<Engine::GameObject*>(this->render_settings.skybox_obj_ptr)->getPropertyPtr<Engine::SkyboxProperty>();
        if (skybox != nullptr)
            skybox->DrawSky(this);
    }
}
