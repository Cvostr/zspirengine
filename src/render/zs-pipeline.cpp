#include "../../headers/render/zs-pipeline.h"
#include "../../headers/engine.h"
#include "../../headers/game.h"
#include "../../headers/world/go_properties.h"
#include "../../headers/world/tile_properties.h"

#define MAX_LIGHTS_AMOUNT 150

extern ZSpireEngine* engine_ptr;

void Engine::RenderSettings::defaults(){
    ambient_light_color = ZSRGBCOLOR(255, 255, 255, 255);
}

void Engine::RenderPipeline::initShaders(){
    this->tile_shader->compileFromFile("Shaders/2d_tile/tile2d.vert", "Shaders/2d_tile/tile2d.frag");
    this->deffered_shader->compileFromFile("Shaders/postprocess/deffered_light/deffered.vert", "Shaders/postprocess/deffered_light/deffered.frag");
    this->default3d->compileFromFile("Shaders/3d/3d.vert", "Shaders/3d/3d.frag");
}

Engine::RenderPipeline::RenderPipeline(){
    this->tile_shader = allocShader();
    this->deffered_shader = allocShader();
    this->default3d = allocShader();
    initShaders();
    //Allocate transform buffer
    this->transformBuffer = allocUniformBuffer();
    transformBuffer->init(0, sizeof (ZSMATRIX4x4) * 3 + 16 * 2);
    //allocate lights buffer
    lightsBuffer = allocUniformBuffer();
    lightsBuffer->init(1, 64 * MAX_LIGHTS_AMOUNT + 16 * 2);
    //Shadow uniform buffer
    shadowBuffer = allocUniformBuffer();
    shadowBuffer->init(2, sizeof (ZSMATRIX4x4) * 2 + 16);
    //Terrain uniform buffer
    terrainUniformBuffer = allocUniformBuffer();
    terrainUniformBuffer->init(3, 12 * 16 * 2 + 4 * 3);
    //Skinning uniform buffer
    skinningUniformBuffer = allocUniformBuffer();
    skinningUniformBuffer->init(4, sizeof (ZSMATRIX4x4) * 150);
    //Tile uniform buffer
    tileBuffer = allocUniformBuffer();
    tileBuffer->init(5, 28);
    //Skybox uniform buffer
    skyboxTransformUniformBuffer = Engine::allocUniformBuffer();
    skyboxTransformUniformBuffer->init(6, sizeof (ZSMATRIX4x4) * 2);

    uiUniformBuffer = Engine::allocUniformBuffer();
    uiUniformBuffer->init(7, sizeof (ZSMATRIX4x4) * 2 + 16 + 16);

    Engine::setupDefaultMeshes();
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
    this->tileBuffer->Destroy();
    this->lightsBuffer->Destroy();
    this->transformBuffer->Destroy();

    tile_shader->Destroy();
    deffered_shader->Destroy();
    default3d->Destroy();

    gbuffer.Destroy();
    //Engine::freeDefaultMeshes();
}

void Engine::RenderPipeline::setLightsToBuffer(){
    this->lightsBuffer->bind();
    for(unsigned int light_i = 0; light_i < this->lights_ptr.size(); light_i ++){
        LightsourceProperty* _light_ptr = static_cast<LightsourceProperty*>(lights_ptr[light_i]);

        lightsBuffer->writeData(64 * light_i, sizeof (int), &_light_ptr->light_type);
        lightsBuffer->writeData(64 * light_i + 4, sizeof (float), &_light_ptr->range);
        lightsBuffer->writeData(64 * light_i + 8, sizeof (float), &_light_ptr->intensity);
        lightsBuffer->writeData(64 * light_i + 12, sizeof (float), &_light_ptr->spot_angle);
        lightsBuffer->writeData(64 * light_i + 16, 12, &_light_ptr->last_pos);
        lightsBuffer->writeData(64 * light_i + 32, 12, &_light_ptr->direction);
        lightsBuffer->writeData(64 * light_i + 48, 4, &_light_ptr->color.gl_r);
        lightsBuffer->writeData(64 * light_i + 52, 4, &_light_ptr->color.gl_g);
        lightsBuffer->writeData(64 * light_i + 56, 4, &_light_ptr->color.gl_b);
    }

    int ls = static_cast<int>(lights_ptr.size());
    lightsBuffer->writeData(64 * MAX_LIGHTS_AMOUNT, 4, &ls);

    ZSVECTOR3 ambient_L = ZSVECTOR3(render_settings.ambient_light_color.r / 255.0f,render_settings.ambient_light_color.g / 255.0f, render_settings.ambient_light_color.b / 255.0f);
    lightsBuffer->writeData(64 * MAX_LIGHTS_AMOUNT + 16, 12, &ambient_L);

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
        case 2:{
            render2D();
            break;
        }
        case 3 :{
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
            GameObject* obj_ptr = &world_ptr->objects[obj_i];
            if(!obj_ptr->hasParent) //if it is a root object
                obj_ptr->processObject(this); //Draw object
        }
    }
}
void Engine::RenderPipeline::render3D(){
    ZSGAME_DATA* game = static_cast<ZSGAME_DATA*>(engine_ptr->getGameDataPtr());
    World* world_ptr = game->world;

    if(engine_ptr->engine_info->graphicsApi == OGL32){
        gbuffer.bindFramebuffer();
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Iterate over all objects in the world
        for(unsigned int obj_i = 0; obj_i < world_ptr->objects.size(); obj_i ++){
            GameObject* obj_ptr = &world_ptr->objects[obj_i];
            if(!obj_ptr->hasParent) //if it is a root object
                obj_ptr->processObject(this); //Draw object
        }

        glDisable(GL_DEPTH_TEST);

        if(cullFaces == true) //if GL face cull is enabled
            glDisable(GL_CULL_FACE);

        glBindFramebuffer(GL_FRAMEBUFFER, 0); //Back to default framebuffer
        glClear(GL_COLOR_BUFFER_BIT); //Clear screen
        gbuffer.bindTextures(); //Bind gBuffer textures
        deffered_shader->Use(); //use deffered shader

        Engine::getPlaneMesh2D()->Draw(); //Draw screen
    }
}

void Engine::GameObject::processObject(RenderPipeline* pipeline){ //On render pipeline wish to work with object
    if(active == false || alive == false) return; //if object is inactive, not to render it

    TransformProperty* transform_prop = static_cast<TransformProperty*>(this->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
    pipeline->transformBuffer->bind();
    pipeline->transformBuffer->writeData(2 * sizeof (ZSMATRIX4x4), sizeof (ZSMATRIX4x4), &transform_prop->transform_mat);

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
            child_ptr->processObject(pipeline);
        }
    }
}
void Engine::GameObject::Draw(RenderPipeline* pipeline){    //On render pipeline wish to draw the object
    //Call prerender on each property in object
    this->onPreRender(pipeline);

    MeshProperty* mesh_prop = static_cast<MeshProperty*>(this->getPropertyPtrByType(GO_PROPERTY_TYPE_MESH));
    //Render all properties
    this->onRender(pipeline);

    if(mesh_prop != nullptr){
        if(mesh_prop->mesh_ptr != nullptr){
            mesh_prop->mesh_ptr->Draw();
        }
    }
}

void Engine::MaterialProperty::onRender(RenderPipeline* pipeline){
    pipeline->default3d->Use();
}

void Engine::TileProperty::onRender(RenderPipeline* pipeline){
    Engine::Shader* tile_shader = pipeline->getTileShader();

    //Receive pointer to tile information
    TileProperty* tile_ptr = static_cast<TileProperty*>(this->go_link.updLinkPtr()->getPropertyPtrByType(GO_PROPERTY_TYPE_TILE));
    TransformProperty* transform_ptr = static_cast<TransformProperty*>(this->go_link.updLinkPtr()->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));

    if(tile_ptr == nullptr || transform_ptr == nullptr) return;

    tile_shader->Use();
       //Bind tile material data
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

    unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
    glDrawBuffers(4, attachments);

    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, 0); //return back to default
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
}

void Engine::G_BUFFER_GL::Destroy(){
    //Remove textures
    glDeleteTextures(1, &tDiffuse);
    glDeleteTextures(1, &tNormal);
    glDeleteTextures(1, &tPos);
    glDeleteTextures(1, &tTransparent);

    //delete framebuffer & renderbuffer
    glDeleteRenderbuffers(1, &this->depthBuffer);
    glDeleteFramebuffers(1, &this->gBuffer);
}

void Engine::RenderPipeline::updateShadersCameraInfo(Engine::Camera* cam_ptr){
    this->transformBuffer->bind();
    ZSMATRIX4x4 proj = cam_ptr->getProjMatrix();
    ZSMATRIX4x4 view = cam_ptr->getViewMatrix();
    ZSVECTOR3 cam_pos = cam_ptr->getCameraPosition();

    transformBuffer->writeData(0, sizeof (ZSMATRIX4x4), &proj);
    transformBuffer->writeData(sizeof (ZSMATRIX4x4), sizeof (ZSMATRIX4x4), &view);
    transformBuffer->writeData(sizeof (ZSMATRIX4x4) * 3, sizeof (cam_pos), &cam_pos);

    //Setting cameras to skybox shader
    proj = cam_ptr->getProjMatrix();
    view = cam_ptr->getViewMatrix();
    view = removeTranslationFromViewMat(view);
    skyboxTransformUniformBuffer->bind();
    skyboxTransformUniformBuffer->writeData(0, sizeof (ZSMATRIX4x4), &proj);
    skyboxTransformUniformBuffer->writeData(sizeof (ZSMATRIX4x4), sizeof (ZSMATRIX4x4), &view);

}

Engine::Shader* Engine::RenderPipeline::getTileShader(){
    return this->tile_shader;
}

void Engine::RenderPipeline::addLight(void* light_ptr){
    this->lights_ptr.push_back(light_ptr);
}

void Engine::RenderPipeline::removeLights(){
    this->lights_ptr.clear();
}

Engine::RenderSettings* Engine::RenderPipeline::getRenderSettings(){
    return &this->render_settings;
}
