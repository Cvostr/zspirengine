#include "../../headers/render/zs-pipeline.h"
#include "../../headers/engine.h"
#include "../../headers/game.h"
#include "../../headers/world/go_properties.h"
#include "../../headers/world/tile_properties.h"

extern ZSpireEngine* engine_ptr;

void Engine::RenderPipeline::initShaders(){
    this->tile_shader.compileFromFile("Shaders/2d_tile/tile2d.vs", "Shaders/2d/tile2d.fs", engine_ptr);
    this->deffered_shader.compileFromFile("Shaders/postprocess/deffered_light/deffered.vs", "Shaders/postprocess/deffered_light/deffered.fs", engine_ptr);
}

Engine::RenderPipeline::RenderPipeline(){

    initShaders();
    Engine::setupDefaultMeshes();

    glDisable(GL_DEPTH_TEST);
    //depthTest = false;
    //cullFaces = false;
    glDisable(GL_CULL_FACE);

    //if we use opengl, then create GBUFFER in GL commands
    if(engine_ptr->engine_info->graphicsApi == OGL32){
        this->gbuffer.create(640, 480);
    }
}

void Engine::RenderPipeline::destroy(){
    tile_shader.Destroy();
    deffered_shader.Destroy();

    gbuffer.Destroy();
    Engine::freeDefaultMeshes();
}

void Engine::RenderPipeline::render(){
    ZSGAME_DATA* game = static_cast<ZSGAME_DATA*>(engine_ptr->getGameDataPtr());

    if(engine_ptr->engine_info->graphicsApi == OGL32){
        gbuffer.bindFramebuffer();
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        World* world_ptr = game->world;

        updateShadersCameraInfo(&world_ptr->cam);

        //Iterate over all objects in the world
        for(unsigned int obj_i = 0; obj_i < world_ptr->objects.size(); obj_i ++){
            GameObject* obj_ptr = &world_ptr->objects[obj_i];
            if(!obj_ptr->hasParent) //if it is a root object
                obj_ptr->processObject(this); //Draw object
        }


        glBindFramebuffer(GL_FRAMEBUFFER, 0); //Back to default framebuffer
        glClear(GL_COLOR_BUFFER_BIT); //Clear screen
        gbuffer.bindTextures(); //Bind gBuffer textures
        deffered_shader.Use(); //use deffered shader

        deffered_shader.setGLuniformVec3("ambient_color", ZSVECTOR3(1.0f,
                                                               1.0f,
                                                               1.0f));

        Engine::getPlaneMesh2D()->Draw(); //Draw screen


    }
}

void Engine::GameObject::processObject(RenderPipeline* pipeline){ //On render pipeline wish to work with object
    //Obtain EditWindow pointer to check if scene is running
    if(active == false || alive == false) return; //if object is inactive, not to render it

    TransformProperty* transform_prop = static_cast<TransformProperty*>(this->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
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

    this->onRender(pipeline);

    if(mesh_prop != nullptr){
        if(mesh_prop->mesh_ptr != nullptr){
            mesh_prop->mesh_ptr->mesh_ptr->Draw();
        }
    }
}

void Engine::TileProperty::onRender(RenderPipeline* pipeline){
    Engine::Shader* tile_shader = pipeline->getTileShader();

    //Receive pointer to tile information
    TileProperty* tile_ptr = static_cast<TileProperty*>(this->go_link.updLinkPtr()->getPropertyPtrByType(GO_PROPERTY_TYPE_TILE));
    TransformProperty* transform_ptr = static_cast<TransformProperty*>(this->go_link.updLinkPtr()->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));

    if(tile_ptr == nullptr || transform_ptr == nullptr) return;

    tile_shader->Use();
    tile_shader->setTransform(transform_ptr->transform_mat);

    //Checking for diffuse texture
    if(tile_ptr->texture_diffuse != nullptr){
        tile_ptr->texture_diffuse->Use(0); //Use this texture
        tile_shader->setHasDiffuseTextureProperty(true); //Shader will use picked diffuse texture

    }else{
        tile_shader->setHasDiffuseTextureProperty(false); //Shader will not use diffuse texture
    }
    //Checking for transparent texture
    if(tile_ptr->texture_transparent != nullptr){
        tile_ptr->texture_transparent->Use(5); //Use this texture
        tile_shader->setGLuniformInt("hasTransparentMap", 1); //Shader will use picked transparent texture

    }else{
        tile_shader->setGLuniformInt("hasTransparentMap", 0); //Shader will not use transparent texture
    }
    //Sending animation info
    if(tile_ptr->anim_property.isAnimated && tile_ptr->anim_state.playing == true){ //If tile animated, then send anim state to shader
        tile_shader->setGLuniformInt("animated", 1); //Send as animated shader
        //Send current animation state
        tile_shader->setGLuniformInt("total_rows", tile_ptr->anim_property.framesX);
        tile_shader->setGLuniformInt("total_cols", tile_ptr->anim_property.framesY);

        tile_shader->setGLuniformInt("selected_row", tile_ptr->anim_state.cur_frameX);
        tile_shader->setGLuniformInt("selected_col", tile_ptr->anim_state.cur_frameY);
    }else{ //No animation or unplayed
         tile_shader->setGLuniformInt("animated", 0);
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
    this->tile_shader.setCamera(cam_ptr);
}

Engine::Shader* Engine::RenderPipeline::getTileShader(){
    return &this->tile_shader;
}
