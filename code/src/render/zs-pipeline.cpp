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
    this->skybox_obj_ptr = nullptr;
    this->shadowcaster_obj_ptr = nullptr;
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
    this->current_state = PIPELINE_STATE::PIPELINE_STATE_DEFAULT;
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
    shadowBuffer->init(2, sizeof (ZSMATRIX4x4) * 4 + 16);
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

    allowOnUpdate = true;
}

Engine::RenderPipeline::~RenderPipeline(){
    destroy();
}

void Engine::RenderPipeline::init(){
    setDepthState(true);

    if(this->game_desc_ptr->game_perspective == PERSP_2D){
        cullFaces = false;
        glDisable(GL_CULL_FACE);
    }else{
        cullFaces = true;
        glEnable(GL_CULL_FACE);
    }
    //if we use opengl, then create GBUFFER in GL commands
    if(this->game_desc_ptr->game_perspective == PERSP_3D){
        create_G_Buffer(this->WIDTH, this->HEIGHT);
    }
}

void Engine::RenderPipeline::create_G_Buffer(unsigned int width, unsigned int height) {
    gbuffer = new GLframebuffer(width, height, true);
    gbuffer->addTexture(GL_RGBA8, GL_RGBA); //Diffuse map
    gbuffer->addTexture(GL_RGB16F, GL_RGB); //Normal map
    gbuffer->addTexture(GL_RGB16F, GL_RGB); //Position map
    gbuffer->addTexture(GL_RGBA, GL_RGBA); //Transparent map
    gbuffer->addTexture(GL_RGBA, GL_RGBA); //Masks map

    df_light_buffer = new GLframebuffer(width, height, false);
    df_light_buffer->addTexture(GL_RGBA, GL_RGBA); //Diffuse map
    df_light_buffer->addTexture(GL_RGB, GL_RGB); //Bloom map
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

        delete gbuffer;
    }
}

void Engine::RenderPipeline::setLightsToBuffer(){
    //Bind Lighting buffer to work with it
    this->lightsBuffer->bind();
    //Iterate over all lights
    for(unsigned int light_i = 0; light_i < this->lights_ptr.size(); light_i ++){
        LightsourceProperty* _light_ptr = static_cast<LightsourceProperty*>(lights_ptr[light_i]);

        LIGHTSOURCE_TYPE light_type = (_light_ptr->light_type);

        lightsBuffer->writeData(LIGHT_STRUCT_SIZE * light_i, sizeof (LIGHTSOURCE_TYPE), &light_type);
        if(_light_ptr->light_type > LIGHTSOURCE_TYPE::LIGHTSOURCE_TYPE_DIRECTIONAL){
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
            render3D(world_ptr->getCameraPtr());
            break;
        }
    }
}

void Engine::RenderPipeline::render2D(){
    ZSGAME_DATA* game = static_cast<ZSGAME_DATA*>(engine_ptr->getGameDataPtr());
    World* world_ptr = game->world;

    if(engine_ptr->engine_info->graphicsApi == OGL32){
        glClearColor(0,0,0,1);
        ClearFBufferGL(true, true);

        //Iterate over all objects in the world
        processObjects(world_ptr);
    }
}
void Engine::RenderPipeline::render3D(Engine::Camera* cam){
    ZSGAME_DATA* game = static_cast<ZSGAME_DATA*>(engine_ptr->getGameDataPtr());
    World* world_ptr = game->world;
    //Render shadows, first
    TryRenderShadows(cam);

    //Bind Geometry Buffer to make Deferred Shading
    gbuffer->bind();
    glClearColor(0,0,0,1);
    ClearFBufferGL(true, true);
    setBlendingState(false);
    setFullscreenViewport(this->WIDTH, this->HEIGHT);

    TryRenderSkybox();
    
    setDepthState(true);
    setFaceCullState(true);
    //Iterate over all objects in the world
    processObjects(world_ptr);
    //Disable depth rendering to draw plane correctly
    setDepthState(false);
    setFaceCullState(false);

    //df_light_buffer->bind();
    glBindFramebuffer(GL_FRAMEBUFFER, 0); //Back to default framebuffer
    ClearFBufferGL(true, false); //Clear screen
    gbuffer->bindTextures(10); //Bind gBuffer textures
    deffered_light->Use(); //use deffered shader

    //df_light_buffer->bindTextures(0);
    Engine::getPlaneMesh2D()->Draw(); //Draw screen

}

void Engine::RenderPipeline::processObjects(void* world_ptr) {
    World* _world_ptr = static_cast<World*>(world_ptr);
    for (unsigned int obj_i = 0; obj_i < _world_ptr->objects.size(); obj_i++) {
        GameObject* obj_ptr = _world_ptr->objects[obj_i];
        if (!obj_ptr->hasParent) //if it is a root object
            obj_ptr->processObject(this); //Draw object
    }
}

void Engine::RenderPipeline::renderDepth(void* world_ptr){
    World* _world_ptr = static_cast<World*>(world_ptr);
    current_state = PIPELINE_STATE::PIPELINE_STATE_SHADOWDEPTH;
    //Iterate over all objects in the world
    processObjects(world_ptr);
    current_state = PIPELINE_STATE::PIPELINE_STATE_DEFAULT;
}

void Engine::GameObject::processObject(RenderPipeline* pipeline) {
    if (alive == false || active == false) return;

    Engine::TransformProperty* transform_prop = this->getTransformProperty();
    //Call update on every property in objects
    if (pipeline->allowOnUpdate && pipeline->current_state == PIPELINE_STATE::PIPELINE_STATE_DEFAULT)
        this->onUpdate(static_cast<int>(pipeline->deltaTime));

    //Obtain camera viewport
    //Engine::ZSVIEWPORT cam_viewport = pipeline->cam->getViewport();
    //Distance limit
    //int max_dist = static_cast<int>(cam_viewport.endX - cam_viewport.startX);
    //bool difts = isDistanceFits(pipeline->cam->getCameraViewCenterPos(), transform_prop->_last_translation, max_dist);

    //if(difts)
    this->Draw(pipeline);

    for (unsigned int obj_i = 0; obj_i < this->children.size(); obj_i++) {
        if (!children[obj_i].isEmpty()) { //if link isn't broken
            children[obj_i].updLinkPtr();
            GameObject* child_ptr = this->children[obj_i].ptr;
            if (child_ptr == nullptr) return;
            child_ptr->processObject(pipeline);
        }
    }
}

void Engine::GameObject::setSkinningMatrices(RenderPipeline* pipeline) {
    MeshProperty* mesh_prop = getPropertyPtr<MeshProperty>();
    //Send all skinning matrices
    if (hasMesh()) {
        //Iterate over all bones
        for (unsigned int bone_i = 0; bone_i < mesh_prop->mesh_ptr->mesh_ptr->bones.size(); bone_i++) {
            //Check for buffer overflow
            if (bone_i >= MAX_MESH_BONES)
                break;
            //Obtain bone by pointer
            Engine::Bone* b = &mesh_prop->mesh_ptr->mesh_ptr->bones[bone_i];

            Engine::GameObject* node = nullptr;
            GameObject* RootNode = mesh_prop->skinning_root_node;
            ZSMATRIX4x4 rootNodeTransform;

            if (RootNode != nullptr) {
                //if RootNode is specified
                node = (mesh_prop->skinning_root_node)->getChildObjectWithNodeLabel(b->bone_name);
                //Get root transform
                rootNodeTransform = (RootNode->getPropertyPtr<Engine::NodeProperty>()->transform_mat);
            }

            if (node != nullptr) {
                Engine::NodeProperty* nd = node->getPropertyPtr<Engine::NodeProperty>();
                //Calculate result matrix
                ZSMATRIX4x4 matrix = transpose(invert(rootNodeTransform) * nd->abs * b->offset);
                //Send skinned matrix to skinning uniform buffer
                pipeline->skinningUniformBuffer->bind();
                pipeline->skinningUniformBuffer->writeData(sizeof(ZSMATRIX4x4) * bone_i, sizeof(ZSMATRIX4x4), &matrix);
            }
        }
    }
}

void Engine::GameObject::Draw(RenderPipeline* pipeline){    //On render pipeline wish to draw the object
    if (pipeline->current_state == PIPELINE_STATE::PIPELINE_STATE_DEFAULT)
        //Call prerender on each property in object
        this->onPreRender(pipeline);
    if (hasMesh() || hasTerrain()) {
        if (pipeline->current_state == PIPELINE_STATE::PIPELINE_STATE_DEFAULT) {
            MeshProperty* mesh_prop = getPropertyPtr<MeshProperty>();
            
            //Render all properties
            this->onRender(pipeline);
            //Send mesh skinning matrices
            setSkinningMatrices(pipeline);
            //Draw mesh
            DrawMesh(pipeline);
        }

        if (pipeline->current_state == PIPELINE_STATE::PIPELINE_STATE_SHADOWDEPTH) {
            Engine::TransformProperty* transform_ptr = getTransformProperty();
            MeshProperty* mesh_prop = getPropertyPtr<MeshProperty>();
            //set transform to camera buffer
            pipeline->transformBuffer->bind();
            pipeline->transformBuffer->writeData(sizeof(ZSMATRIX4x4) * 2, sizeof(ZSMATRIX4x4), &transform_ptr->transform_mat);
            //Send mesh skinning matrices
            setSkinningMatrices(pipeline);
            
            //Get castShadows boolean from several properties
            bool castShadows = (hasTerrain()) ? getPropertyPtr<TerrainProperty>()->castShadows : mesh_prop->castShadows;
            //If castShadows is true, then render mesh to DepthBuffer
            if(castShadows)
                DrawMesh(pipeline);
        }
    }
}

void Engine::MaterialProperty::onRender(Engine::RenderPipeline* pipeline){
    //Check for validity of pointer
    if(material_ptr == nullptr) return;

    MtShaderPropertiesGroup* group_ptr = material_ptr->group_ptr;
    if( group_ptr == nullptr) return ; //if object hasn't property

    //Get pointer to shadowcaster
    Engine::ShadowCasterProperty* shadowcast = nullptr;
    if (pipeline->getRenderSettings()->shadowcaster_obj_ptr != nullptr) {
        shadowcast =
            static_cast<Engine::GameObject*>(pipeline->getRenderSettings()->shadowcaster_obj_ptr)->getPropertyPtr<Engine::ShadowCasterProperty>();
        //Bind shadow uniform buffer
        pipeline->shadowBuffer->bind();
    }

    int recShadows = 1;

    if(shadowcast == nullptr){
        //In GLSL we should use Integer instead of bool
        recShadows = 0;
    }else if(!receiveShadows || !shadowcast->isActive()){
        recShadows = 0;
    }else
        shadowcast->setTexture();

    pipeline->shadowBuffer->writeData(sizeof (ZSMATRIX4x4) * 4 + 4, 4, &recShadows);
    //Apply matrerial to shader and textures
    material_ptr->applyMatToPipeline();
}

void Engine::SkyboxProperty::onPreRender(Engine::RenderPipeline* pipeline){
    pipeline->getRenderSettings()->skybox_obj_ptr = static_cast<void*>(this->go_link.updLinkPtr());
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

    ZSMATRIX4x4 translation = getTranslationMat(static_cast<float>(X), static_cast<float>(Y), 0.0f);
    ZSMATRIX4x4 scale = getScaleMat(static_cast<float>(scaleX), static_cast<float>(scaleY), 0.0f);
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

    ZSMATRIX4x4 translation = getTranslationMat(static_cast<float>(X), static_cast<float>(Y), 0.0f);
    ZSMATRIX4x4 scale = getScaleMat(static_cast<float>(scaleX), static_cast<float>(scaleY), 0.0f);
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

Engine::Shader* Engine::RenderPipeline::getUiShader() {
    return this->ui_shader;
}

void Engine::RenderPipeline::addLight(void* light_ptr){
    this->lights_ptr.push_back(light_ptr);
}

void Engine::RenderPipeline::removeLights(){
    this->lights_ptr.clear();
}

void Engine::RenderPipeline::updateWindowSize(int W, int H){
     setFullscreenViewport(W, H);
     delete gbuffer;
     create_G_Buffer(W, H);
}

Engine::RenderSettings* Engine::RenderPipeline::getRenderSettings(){
    return &this->render_settings;
}

void Engine::RenderPipeline::setBlendingState(bool blend) {
    if(blend)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
}
void Engine::RenderPipeline::setDepthState(bool depth) {
    if (depth)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}
void Engine::RenderPipeline::setFaceCullState(bool face_cull) {
    if (face_cull)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);
}
void Engine::RenderPipeline::setFullscreenViewport(unsigned int Width, unsigned int Height) {
    glViewport(0, 0, Width, Height);
}

void Engine::RenderPipeline::ClearFBufferGL(bool clearColor, bool clearDepth) {
    GLbitfield clearMask = 0;
    if (clearColor)
        clearMask |= GL_COLOR_BUFFER_BIT;
    if (clearDepth)
        clearMask |= GL_DEPTH_BUFFER_BIT;

    glClear(clearMask);
}

void Engine::RenderPipeline::TryRenderShadows(Engine::Camera* cam) {
    if (this->render_settings.shadowcaster_obj_ptr != nullptr) {
        Engine::ShadowCasterProperty* shadowcast =
            static_cast<Engine::GameObject*>(this->render_settings.shadowcaster_obj_ptr)->getPropertyPtr<Engine::ShadowCasterProperty>();
        if (shadowcast != nullptr) { //we have shadowcaster
            shadowcast->Draw(cam, this); //draw shadowcaster
        }
    }
}
void Engine::RenderPipeline::TryRenderSkybox() {
    if (this->render_settings.skybox_obj_ptr != nullptr) {
        Engine::SkyboxProperty* skybox
            = static_cast<Engine::GameObject*>(this->render_settings.skybox_obj_ptr)->getPropertyPtr<Engine::SkyboxProperty>();
        if (skybox != nullptr)
            skybox->DrawSky(this);
    }
}

void Engine::GLframebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbuffer);
    glViewport(0, 0, Width, Height);
}

void Engine::GLframebuffer::bindTextures(unsigned int m) {
    for (unsigned int t = 0; t < texture_size; t++) {
        glActiveTexture(GL_TEXTURE0 + m + t);
        glBindTexture(GL_TEXTURE_2D, textures[t]);
    }
}

void Engine::GLframebuffer::addTexture(GLint intFormat, GLint format) {
    if (texture_size == MAX_RENDERER_ATTACHMENT_COUNT) return;
    bind();
    //Create texture
    glGenTextures(1, &textures[texture_size]);
    glBindTexture(GL_TEXTURE_2D, textures[texture_size]);
    glTexImage2D(GL_TEXTURE_2D, 0, intFormat, Width, Height, 0, format, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + texture_size, GL_TEXTURE_2D, textures[texture_size], 0);
    texture_size++; //Add texture

    unsigned int attachments[MAX_RENDERER_ATTACHMENT_COUNT] =
    { GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1,
    GL_COLOR_ATTACHMENT2,
    GL_COLOR_ATTACHMENT3,
    GL_COLOR_ATTACHMENT4,
    GL_COLOR_ATTACHMENT5,
    GL_COLOR_ATTACHMENT6 };

    glDrawBuffers(texture_size, attachments);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Engine::GLframebuffer::~GLframebuffer() {
    if (Depth)
        glDeleteRenderbuffers(1, &this->depthBuffer);
    glDeleteFramebuffers(1, &this->fbuffer);

    for (unsigned int t = 0; t < texture_size; t++) {
        glDeleteTextures(1, &textures[t]);
    }
}
Engine::GLframebuffer::GLframebuffer(unsigned int width, unsigned int height, bool depth) {
    textures[0] = 0;
    this->Width = width;
    this->Height = height;

    this->Depth = depth;
    this->depthBuffer = 0;
    texture_size = 0;

    glGenFramebuffers(1, &fbuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, fbuffer);
    if (depth) {
        glGenRenderbuffers(1, &depthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0); //return back to default
}
