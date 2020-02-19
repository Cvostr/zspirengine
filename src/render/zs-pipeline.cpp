#include "../../headers/render/zs-pipeline.h"
#include "../../headers/engine.h"
#include "../../headers/game.h"
#include "../../headers/world/go_properties.h"
#include "../../headers/world/tile_properties.h"

#define LIGHT_STRUCT_SIZE 64
extern ZSpireEngine* engine_ptr;

void Engine::RenderSettings::defaults(){
    ambient_light_color = ZSRGBCOLOR(255, 255, 255, 255);
    resetPointers();
}

void Engine::RenderSettings::resetPointers(){
    this->skybox_ptr = nullptr;
    this->shadowcaster_ptr = nullptr;
}

void Engine::RenderPipeline::initShaders(){
    this->ui_shader = Engine::allocShader();
    this->ui_shader->compileFromFile("Shaders/ui/ui.vert", "Shaders/ui/ui.frag");

    if(engine_ptr->desc->game_perspective == PERSP_2D){
        this->tile_shader = allocShader();
        this->tile_shader->compileFromFile("Shaders/2d_tile/tile2d.vert", "Shaders/2d_tile/tile2d.frag");
    }
    if(engine_ptr->desc->game_perspective == PERSP_3D){
        this->default3d = allocShader();
        this->deffered_light = allocShader();
        this->skybox_shader = allocShader();
        this->terrain_shader = allocShader();
        this->grass_shader = Engine::allocShader();
        this->shadowMap = Engine::allocShader();

        this->deffered_light->compileFromFile("Shaders/postprocess/deffered_light/deffered.vert", "Shaders/postprocess/deffered_light/deffered.frag");
        this->default3d->compileFromFile("Shaders/3d/3d.vert", "Shaders/3d/3d.frag");
        this->skybox_shader->compileFromFile("Shaders/skybox/skybox.vert", "Shaders/skybox/skybox.frag");
        this->terrain_shader->compileFromFile("Shaders/heightmap/heightmap.vert", "Shaders/heightmap/heightmap.frag");
        this->grass_shader->compileFromFile("Shaders/heightmap/grass.vert", "Shaders/heightmap/grass.frag");
        this->shadowMap->compileFromFile("Shaders/shadowmap/shadowmap.vert", "Shaders/shadowmap/shadowmap.frag");

        MtShProps::genDefaultMtShGroup(default3d, skybox_shader, terrain_shader, 9);
    }
}

Engine::RenderPipeline::RenderPipeline(){
    this->current_state = PIPELINE_STATE_DEFAULT;
    this->cullFaces = false;

    initShaders();
    //Allocate transform buffer
    this->transformBuffer = allocUniformBuffer();
    transformBuffer->init(0, sizeof (ZSMATRIX4x4) * 3 + 16 * 2);
    //allocate lights buffer
    lightsBuffer = allocUniformBuffer();
    lightsBuffer->init(1, LIGHT_STRUCT_SIZE * MAX_LIGHTS_AMOUNT + 16 * 2);
    //Shadow uniform buffer
    shadowBuffer = allocUniformBuffer();
    shadowBuffer->init(2, sizeof (ZSMATRIX4x4) * 2 + 16);
    //Terrain uniform buffer
    terrainUniformBuffer = allocUniformBuffer();
    terrainUniformBuffer->init(3, 12 * 16 * 2 + 4 * 3);
    {
        //Skinning uniform buffer
        skinningUniformBuffer = allocUniformBuffer();
        skinningUniformBuffer->init(4, sizeof (ZSMATRIX4x4) * MAX_MESH_BONES);
        for(unsigned int i = 0; i < MAX_MESH_BONES; i ++){
            ZSMATRIX4x4 m = getIdentity();
            skinningUniformBuffer->writeData(sizeof (ZSMATRIX4x4) * i, sizeof (ZSMATRIX4x4), &m);
        }
    }
    //Tile uniform buffer
    tileBuffer = allocUniformBuffer();
    tileBuffer->init(5, 28);
    //Skybox uniform buffer
    skyboxTransformUniformBuffer = Engine::allocUniformBuffer();
    skyboxTransformUniformBuffer->init(6, sizeof (ZSMATRIX4x4) * 2);

    uiUniformBuffer = Engine::allocUniformBuffer();
    uiUniformBuffer->init(7, sizeof (ZSMATRIX4x4) * 2 + 16 + 16);

    {
        instancedTransformBuffer = Engine::allocUniformBuffer();
        instancedTransformBuffer->init(9, sizeof (ZSMATRIX4x4) * INSTANCED_RENDER_BUFFER_SIZE);

        for(unsigned int i = 0; i < INSTANCED_RENDER_BUFFER_SIZE; i ++){
            ZSMATRIX4x4 m = getIdentity();
            instancedTransformBuffer->writeData(sizeof (ZSMATRIX4x4) * i, sizeof (ZSMATRIX4x4), &m);
        }
    }


    Engine::setupDefaultMeshes();
    //to avoid memory reallocations
    this->lights_ptr.reserve(MAX_LIGHTS_AMOUNT);
}

Engine::RenderPipeline::~RenderPipeline(){
    destroy();
}

void Engine::RenderPipeline::init(){

    if(this->game_desc_ptr->game_perspective == PERSP_2D){
        glDisable(GL_DEPTH_TEST);
        depthTest = false;
        cullFaces = false;
        glDisable(GL_CULL_FACE);
    }else{
        glEnable(GL_DEPTH_TEST);
        depthTest = true;
        cullFaces = true;
        glEnable(GL_CULL_FACE);
    }
    //if we use opengl, then create GBUFFER in GL commands
    if(engine_ptr->engine_info->graphicsApi == OGL32 && this->game_desc_ptr->game_perspective == PERSP_3D){
        this->gbuffer.create(this->WIDTH, this->HEIGHT);
    }
}

void Engine::RenderPipeline::destroy(){
    if(engine_ptr->desc->game_perspective == PERSP_2D){
        this->tileBuffer->Destroy();
        tile_shader->Destroy();
    }

    ui_shader->Destroy();

    this->lightsBuffer->Destroy();
    this->tileBuffer->Destroy();
    this->transformBuffer->Destroy();
    this->terrainUniformBuffer->Destroy();
    this->skinningUniformBuffer->Destroy();
    this->skyboxTransformUniformBuffer->Destroy();
    this->instancedTransformBuffer->Destroy();

    if(engine_ptr->desc->game_perspective == PERSP_3D){
        deffered_light->Destroy();
        default3d->Destroy();
        skybox_shader->Destroy();
        terrain_shader->Destroy();
        grass_shader->Destroy();
        shadowMap->Destroy();

        gbuffer.Destroy();
    }

    freeDefaultMeshes();
}

void Engine::RenderPipeline::setLightsToBuffer(){
    this->lightsBuffer->bind();
    for(unsigned int light_i = 0; light_i < this->lights_ptr.size(); light_i ++){
        LightsourceProperty* _light_ptr = static_cast<LightsourceProperty*>(lights_ptr[light_i]);

        LIGHTSOURCE_TYPE light_type = (_light_ptr->light_type);

        lightsBuffer->writeData(LIGHT_STRUCT_SIZE * light_i, sizeof (LIGHTSOURCE_TYPE), &light_type);
        if(_light_ptr->light_type > LIGHTSOURCE_TYPE_DIRECTIONAL){
            lightsBuffer->writeData(LIGHT_STRUCT_SIZE * light_i + 4, sizeof (float), &_light_ptr->range);
            lightsBuffer->writeData(LIGHT_STRUCT_SIZE * light_i + 12, sizeof (float), &_light_ptr->spot_angle);
        }
        lightsBuffer->writeData(LIGHT_STRUCT_SIZE * light_i + 8, sizeof (float), &_light_ptr->intensity);

        lightsBuffer->writeData(LIGHT_STRUCT_SIZE * light_i + 16, 12, &_light_ptr->last_pos);
        lightsBuffer->writeData(LIGHT_STRUCT_SIZE * light_i + 32, 12, &_light_ptr->direction);
        lightsBuffer->writeData(LIGHT_STRUCT_SIZE * light_i + 48, sizeof (int), &_light_ptr->color.gl_r);
        lightsBuffer->writeData(LIGHT_STRUCT_SIZE * light_i + 52, sizeof (int), &_light_ptr->color.gl_g);
        lightsBuffer->writeData(LIGHT_STRUCT_SIZE * light_i + 56, sizeof (int), &_light_ptr->color.gl_b);
    }

    int ls = static_cast<int>(lights_ptr.size());
    lightsBuffer->writeData(LIGHT_STRUCT_SIZE * MAX_LIGHTS_AMOUNT, 4, &ls);

    ZSVECTOR3 ambient_L = ZSVECTOR3(render_settings.ambient_light_color.r / 255.0f,render_settings.ambient_light_color.g / 255.0f, render_settings.ambient_light_color.b / 255.0f);
    lightsBuffer->writeData(LIGHT_STRUCT_SIZE * MAX_LIGHTS_AMOUNT + 16, 12, &ambient_L);

    //free lights array
    this->removeLights();
}

void Engine::RenderPipeline::render(){
    ZSGAME_DATA* game = static_cast<ZSGAME_DATA*>(engine_ptr->getGameDataPtr());
    World* world_ptr = game->world;
    //send lights to uniform buffer
    setLightsToBuffer();
    //set camera data to transform buffer
    updateShadersCameraInfo(world_ptr->getCameraPtr());

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

void Engine::RenderPipeline::render2D(){
    ZSGAME_DATA* game = static_cast<ZSGAME_DATA*>(engine_ptr->getGameDataPtr());
    World* world_ptr = game->world;

    if(engine_ptr->engine_info->graphicsApi == OGL32){
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Iterate over all objects in the world
        for(unsigned int obj_i = 0; obj_i < world_ptr->objects.size(); obj_i ++){
            GameObject* obj_ptr = world_ptr->objects[obj_i];
            if(!obj_ptr->hasParent) //if it is a root object
                obj_ptr->processObject(this); //Draw object
        }
    }
}
void Engine::RenderPipeline::render3D(){
    ZSGAME_DATA* game = static_cast<ZSGAME_DATA*>(engine_ptr->getGameDataPtr());
    World* world_ptr = game->world;

    Engine::ShadowCasterProperty* shadowcast = static_cast<Engine::ShadowCasterProperty*>(this->render_settings.shadowcaster_ptr);
    if(shadowcast != nullptr){ //we have shadowcaster
        shadowcast->Draw(&world_ptr->world_camera, this); //draw shadowcaster
    }

    if(engine_ptr->engine_info->graphicsApi == OGL32){
        gbuffer.bindFramebuffer();
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Iterate over all objects in the world
        for(unsigned int obj_i = 0; obj_i < world_ptr->objects.size(); obj_i ++){
            GameObject* obj_ptr = world_ptr->objects[obj_i];
            if(!obj_ptr->hasParent) //if it is a root object
                obj_ptr->processObject(this); //Draw object
        }

        glDisable(GL_DEPTH_TEST);

        if(cullFaces == true) //if GL face cull is enabled
            glDisable(GL_CULL_FACE);

        glBindFramebuffer(GL_FRAMEBUFFER, 0); //Back to default framebuffer
        glClear(GL_COLOR_BUFFER_BIT); //Clear screen
        gbuffer.bindTextures(); //Bind gBuffer textures
        deffered_light->Use(); //use deffered shader

        Engine::getPlaneMesh2D()->Draw(); //Draw screen
    }
}

void Engine::RenderPipeline::renderDepth(void* world_ptr){
    World* _world_ptr = static_cast<World*>(world_ptr);
    current_state = PIPELINE_STATE_SHADOWDEPTH;
    //Iterate over all objects in the world
    for(unsigned int obj_i = 0; obj_i < _world_ptr->objects.size(); obj_i ++){
        GameObject* obj_ptr = (GameObject*)_world_ptr->objects[obj_i];
        if(!obj_ptr->hasParent) //if it is a root object
            obj_ptr->processObject(this); //Draw object
    }
    current_state = PIPELINE_STATE_DEFAULT;
}

void Engine::GameObject::processObject(RenderPipeline* pipeline){ //On render pipeline wish to work with object
    if(active == false || alive == false) return; //if object is inactive, not to render it

    TransformProperty* transform_prop = getTransformProperty();

    if(pipeline->current_state == PIPELINE_STATE_DEFAULT)
        //Call update on every property in objects
        this->onUpdate(static_cast<int>(pipeline->deltaTime));

    //Obtain camera viewport
    //ZSVIEWPORT cam_viewport = pipeline->cam->getViewport();
    //Distance limit
    //int max_dist = static_cast<int>(cam_viewport.endX - cam_viewport.startX);
   // bool difts = isDistanceFits(pipeline->cam->getCameraViewCenterPos(), transform_prop->_last_translation, max_dist);

   // if(difts)
        this->Draw(pipeline);

    for(unsigned int obj_i = 0; obj_i < this->children.size(); obj_i ++){
        if(!children[obj_i].isEmpty()){ //if link isn't broken
            children[obj_i].updLinkPtr();
            GameObject* child_ptr = this->children[obj_i].ptr;
            if(child_ptr == nullptr) continue;
            child_ptr->processObject(pipeline);
        }
    }
}
void Engine::GameObject::Draw(RenderPipeline* pipeline){    //On render pipeline wish to draw the object
    //Call prerender on each property in object
    this->onPreRender(pipeline);

    if(pipeline->current_state == PIPELINE_STATE_DEFAULT) {
        MeshProperty* mesh_prop = static_cast<MeshProperty*>(this->getPropertyPtrByType(GO_PROPERTY_TYPE_MESH));
        //Render all properties
        this->onRender(pipeline);

        if(mesh_prop != nullptr){
            if(mesh_prop->mesh_ptr != nullptr){
                mesh_prop->mesh_ptr->Draw();
            }
        }
    }

    if(pipeline->current_state == PIPELINE_STATE_SHADOWDEPTH) {
        Engine::TransformProperty* transform_ptr = static_cast<Engine::TransformProperty*>(getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
        MeshProperty* mesh_prop = static_cast<MeshProperty*>(this->getPropertyPtrByType(GO_PROPERTY_TYPE_MESH));
        //set transform to camera buffer
        pipeline->transformBuffer->bind();
        pipeline->transformBuffer->writeData(sizeof (ZSMATRIX4x4) * 2, sizeof (ZSMATRIX4x4), &transform_ptr->transform_mat);

        //Get castShadows boolean from several properties
        //bool castShadows = (hasTerrain()) ? getPropertyPtr<TerrainProperty>()->castShadows : mesh_prop->castShadows;

        //if(castShadows)
          //  DrawMesh(pipeline);
        if(mesh_prop != nullptr){
            if(mesh_prop->mesh_ptr != nullptr){
                mesh_prop->mesh_ptr->Draw();
            }
        }
    }
}

void Engine::MaterialProperty::onRender(Engine::RenderPipeline* pipeline){
    //Check for validity of pointer
    if(material_ptr == nullptr) return;

    MtShaderPropertiesGroup* group_ptr = material_ptr->group_ptr;
    if( group_ptr == nullptr) return ; //if object hasn't property

    //Get pointer to shadowcaster
    Engine::ShadowCasterProperty* shadowcast = static_cast<Engine::ShadowCasterProperty*>(pipeline->getRenderSettings()->shadowcaster_ptr);
    //Bind shadow uniform buffer
    pipeline->shadowBuffer->bind();

    int recShadows = 1;

    if(shadowcast == nullptr){
        //In GLSL we should use Integer instead of bool
        recShadows = 0;
    }else if(!receiveShadows || !shadowcast->isActive()){
        recShadows = 0;
    }else
        shadowcast->setTexture();

    pipeline->shadowBuffer->writeData(sizeof (ZSMATRIX4x4) * 2 + 4, 4, &recShadows);

    material_ptr->applyMatToPipeline();
}

void Engine::SkyboxProperty::onPreRender(Engine::RenderPipeline* pipeline){
    pipeline->getRenderSettings()->skybox_ptr = static_cast<void*>(this);
}

void Engine::ShadowCasterProperty::onPreRender(Engine::RenderPipeline* pipeline){
    pipeline->getRenderSettings()->shadowcaster_ptr = static_cast<void*>(this);
}

void Engine::SkyboxProperty::DrawSky(RenderPipeline* pipeline){
    if(!this->isActive())
        return;
    if(this->go_link.updLinkPtr() == nullptr) return;
    //Get pointer to Material property
    MaterialProperty* mat = this->go_link.updLinkPtr()->getPropertyPtr<MaterialProperty>();
    if(mat == nullptr) return;
    //Apply material shader
    mat->onRender(pipeline);
    //Draw skybox cube
    glDisable(GL_DEPTH_TEST);
    Engine::getSkyboxMesh()->Draw();
}

void Engine::ShadowCasterProperty::init(){
    glGenFramebuffers(1, &this->shadowBuffer);//Generate framebuffer for texture
    glGenTextures(1, &this->shadowDepthTexture); //Generate texture

    glBindTexture(GL_TEXTURE_2D, shadowDepthTexture);
    //Configuring texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, this->TextureWidth, this->TextureHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    //Binding framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
    //Connecting depth texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->shadowDepthTexture, 0);
    //We won't render color
    glDrawBuffer(false);
    glReadBuffer(false);
    //Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    this->initialized = true;
}

void Engine::ShadowCasterProperty::Draw(Engine::Camera* cam, RenderPipeline* pipeline){
    if(!isRenderAvailable()){
        glViewport(0, 0, TextureWidth, TextureHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer); //Bind framebuffer
        glClear(GL_DEPTH_BUFFER_BIT);
        return;
    }

    Engine::LightsourceProperty* light = this->go_link.updLinkPtr()->getPropertyPtr<Engine::LightsourceProperty>();

    //ZSVECTOR3 cam_pos = cam->getCameraPosition() + cam->getCameraFrontVec() * 20;
    ZSVECTOR3 cam_pos = cam->getCameraPosition() - light->direction * 20;
    this->LightProjectionMat = getOrthogonal(-projection_viewport, projection_viewport, -projection_viewport, projection_viewport, nearPlane, farPlane);
    this->LightViewMat = matrixLookAt(cam_pos, cam_pos + light->direction * -1, ZSVECTOR3(0,1,0));
    //Changing viewport to texture sizes
    glViewport(0, 0, TextureWidth, TextureHeight);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer); //Bind framebuffer
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CW);
    glDisable(GL_CULL_FACE);
    //Bind shadow uniform buffer
    pipeline->shadowBuffer->bind();
    //Bind ortho shadow projection
    pipeline->shadowBuffer->writeData(0, sizeof (ZSMATRIX4x4), &LightProjectionMat);
    //Bind shadow view matrix
    pipeline->shadowBuffer->writeData(sizeof (ZSMATRIX4x4), sizeof (ZSMATRIX4x4), &LightViewMat);
    //Send BIAS value
    pipeline->shadowBuffer->writeData(sizeof (ZSMATRIX4x4) * 2, 4, &shadow_bias);
    //Send Width of shadow texture
    pipeline->shadowBuffer->writeData(sizeof (ZSMATRIX4x4) * 2 + 8, 4, &this->TextureWidth);
    //Send Height of shadow texture
    pipeline->shadowBuffer->writeData(sizeof (ZSMATRIX4x4) * 2 + 12, 4, &this->TextureHeight);
    //Use shadowmap shader to draw objects
    pipeline->getShadowmapShader()->Use();
    //Render to depth all scene
    pipeline->renderDepth(this->go_link.world_ptr);

    glFrontFace(GL_CCW);
}

void Engine::TileProperty::onRender(Engine::RenderPipeline* pipeline){
    Engine::Shader* tile_shader = pipeline->getTileShader();

    tile_shader->Use();

    pipeline->tileBuffer->bind();

    //Checking for diffuse texture
    if(texture_diffuse != nullptr){
        texture_diffuse->Use(0); //Use this texture
    }

    int diffuse1_ = texture_diffuse != nullptr;
    pipeline->tileBuffer->writeData(20, 4, &diffuse1_);

    //Checking for transparent texture
    if(texture_transparent != nullptr){
        texture_transparent->Use(1); //Use this texture
    }
    int diffuse2_ = texture_transparent != nullptr;
    pipeline->tileBuffer->writeData(24, 4, &diffuse2_);
    //calculate animation state
    int anim_state_i = anim_property.isAnimated && anim_state.playing;
    //Sending animation info
    if(anim_property.isAnimated && anim_state.playing == true){ //If tile animated, then send anim state to shader
        pipeline->tileBuffer->writeData(16, 4, &anim_state_i);
        pipeline->tileBuffer->writeData(0, 4, &anim_property.framesX);
        pipeline->tileBuffer->writeData(4, 4, &anim_property.framesY);
        pipeline->tileBuffer->writeData(8, 4, &anim_state.cur_frameX);
        pipeline->tileBuffer->writeData(12, 4, &anim_state.cur_frameY);
    }else{ //No animation or unplayed
        pipeline->tileBuffer->writeData(16, 4, &anim_state_i);
    }
}


Engine::G_BUFFER_GL::G_BUFFER_GL(){
    created = false;
}

void Engine::G_BUFFER_GL::create(int width, int height){
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    glGenTextures(1, &tDiffuse);
    glBindTexture(GL_TEXTURE_2D, tDiffuse);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tDiffuse, 0);

    glGenTextures(1, &tNormal);
    glBindTexture(GL_TEXTURE_2D, tNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tNormal, 0);

    glGenTextures(1, &tPos);
    glBindTexture(GL_TEXTURE_2D, tPos);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, tPos, 0);

    glGenTextures(1, &tTransparent);
    glBindTexture(GL_TEXTURE_2D, tTransparent);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, tTransparent, 0);

    glGenTextures(1, &tMasks);
    glBindTexture(GL_TEXTURE_2D, tMasks);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, tMasks, 0);

    unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4};
    glDrawBuffers(5, attachments);

    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, 0); //return back to default

    created = true;
}
void Engine::G_BUFFER_GL::bindFramebuffer(){
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
}
void Engine::G_BUFFER_GL::bindTextures(){
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, tDiffuse);

    glActiveTexture(GL_TEXTURE11);
    glBindTexture(GL_TEXTURE_2D, tNormal);

    glActiveTexture(GL_TEXTURE12);
    glBindTexture(GL_TEXTURE_2D, tPos);

    glActiveTexture(GL_TEXTURE13);
    glBindTexture(GL_TEXTURE_2D, tTransparent);

    glActiveTexture(GL_TEXTURE14);
    glBindTexture(GL_TEXTURE_2D, tMasks);
}

void Engine::G_BUFFER_GL::Destroy(){
    //Remove textures
    glDeleteTextures(1, &tDiffuse);
    glDeleteTextures(1, &tNormal);
    glDeleteTextures(1, &tPos);
    glDeleteTextures(1, &tTransparent);
    glDeleteTextures(1, &tMasks);

    //delete framebuffer & renderbuffer
    glDeleteRenderbuffers(1, &this->depthBuffer);
    glDeleteFramebuffers(1, &this->gBuffer);
}

void Engine::RenderPipeline::updateShadersCameraInfo(Engine::Camera* cam_ptr){
    transformBuffer->bind();
    ZSMATRIX4x4 proj = cam_ptr->getProjMatrix();
    ZSMATRIX4x4 view = cam_ptr->getViewMatrix();
    ZSVECTOR3 cam_pos = cam_ptr->getCameraPosition();
    transformBuffer->writeData(0, sizeof (ZSMATRIX4x4), &proj);
    transformBuffer->writeData(sizeof (ZSMATRIX4x4), sizeof (ZSMATRIX4x4), &view);
    transformBuffer->writeData(sizeof (ZSMATRIX4x4) * 3, sizeof(ZSVECTOR3), &cam_pos);
    //Setting UI camera to UI buffer
    uiUniformBuffer->bind();
    proj = cam_ptr->getUiProjMatrix();
    uiUniformBuffer->writeData(0, sizeof (ZSMATRIX4x4), &proj);
    //Setting cameras to skybox shader
    proj = cam_ptr->getProjMatrix();
    view = cam_ptr->getViewMatrix();
    view = removeTranslationFromViewMat(view);
    skyboxTransformUniformBuffer->bind();
    skyboxTransformUniformBuffer->writeData(0, sizeof (ZSMATRIX4x4), &proj);
    skyboxTransformUniformBuffer->writeData(sizeof (ZSMATRIX4x4), sizeof (ZSMATRIX4x4), &view);
}

void Engine::RenderPipeline::renderSprite(Engine::Texture* texture_sprite, int X, int Y, int scaleX, int scaleY){
    this->ui_shader->Use();
    uiUniformBuffer->bind();

    int _render_mode = 1;
    uiUniformBuffer->writeData(sizeof (ZSMATRIX4x4) * 2 , 4, &_render_mode);
    //Use texture at 0 slot
    texture_sprite->Use(0);

    ZSMATRIX4x4 translation = getTranslationMat(X, Y, 0.0f);
    ZSMATRIX4x4 scale = getScaleMat(scaleX, scaleY, 0.0f);
    ZSMATRIX4x4 transform = scale * translation;

    //Push glyph transform
    uiUniformBuffer->writeData(sizeof (ZSMATRIX4x4), sizeof (ZSMATRIX4x4), &transform);

    Engine::getUiSpriteMesh2D()->Draw();
}

void Engine::RenderPipeline::renderSprite(Engine::TextureResource* texture_sprite, int X, int Y, int scaleX, int scaleY){
    texture_sprite->Use(0);
    renderSprite(texture_sprite->texture_ptr, X, Y, scaleX, scaleY);
}

void Engine::RenderPipeline::renderGlyph(unsigned int texture_id, int X, int Y, int scaleX, int scaleY, ZSRGBCOLOR color){
    this->ui_shader->Use();
    uiUniformBuffer->bind();
    //tell shader, that we will render glyph
    int _render_mode = 2;
    uiUniformBuffer->writeData(sizeof (ZSMATRIX4x4) * 2 , 4, &_render_mode);
    //sending glyph color
    uiUniformBuffer->writeData(sizeof (ZSMATRIX4x4) * 2 + 16, 4, &color.gl_r);
    uiUniformBuffer->writeData(sizeof (ZSMATRIX4x4) * 2 + 4 + 16, 4, &color.gl_g);
    uiUniformBuffer->writeData(sizeof (ZSMATRIX4x4) * 2 + 8 + 16, 4, &color.gl_b);

    //Use texture at 0 slot
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    ZSMATRIX4x4 translation = getTranslationMat(X, Y, 0.0f);
    ZSMATRIX4x4 scale = getScaleMat(scaleX, scaleY, 0.0f);
    ZSMATRIX4x4 transform = scale * translation;

    //Push glyph transform
    uiUniformBuffer->writeData(sizeof (ZSMATRIX4x4), sizeof (ZSMATRIX4x4), &transform);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    Engine::getUiSpriteMesh2D()->Draw();
}


Engine::Shader* Engine::RenderPipeline::getTileShader(){
    return this->tile_shader;
}

Engine::Shader* Engine::RenderPipeline::getShadowmapShader(){
    return this->shadowMap;
}

void Engine::RenderPipeline::addLight(void* light_ptr){
    this->lights_ptr.push_back(light_ptr);
}

void Engine::RenderPipeline::removeLights(){
    this->lights_ptr.clear();
}

void Engine::RenderPipeline::updateWindowSize(int W, int H){
     glViewport(0, 0, W, H);
    //Recreate gBuffer with new resolution
    if(gbuffer.created){
        this->gbuffer.Destroy();
        this->gbuffer.create(W, H);
    }
}

Engine::RenderSettings* Engine::RenderPipeline::getRenderSettings(){
    return &this->render_settings;
}
