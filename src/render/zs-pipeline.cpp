#include "../../headers/render/zs-pipeline.h"
#include "../../headers/engine.h"
#include "../../headers/game.h"
#include "../../headers/world/go_properties.h"

extern ZSpireEngine* engine_ptr;

void Engine::RenderPipeline::initShaders(){
    this->tile_shader.compileFromFile("Shaders/2d_tile/tile2d.vs", "Shaders/2d/tile2d.fs", engine_ptr);
    this->deffered_shader.compileFromFile("Shaders/postprocess/deffered_light/deffered.vs", "Shaders/postprocess/deffered_light/deffered.fs", engine_ptr);
}

Engine::RenderPipeline::RenderPipeline(){

    initShaders();
    Engine::setupDefaultMeshes();

    //if we use opengl, then create GBUFFER in GL commands
    if(engine_ptr->engine_info->graphicsApi == OGL32){
        this->gbuffer.create(640, 480);
    }
}

void Engine::RenderPipeline::render(){
    ZSGAME_DATA* game = static_cast<ZSGAME_DATA*>(engine_ptr->getGameDataPtr());

    if(engine_ptr->engine_info->graphicsApi == OGL32){
        gbuffer.bindFramebuffer();
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        World* world_ptr = game->world;
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

        SDL_GL_SwapWindow(engine_ptr->getWindowSDL()); //Send rendered frame
    }
}

void Engine::GameObject::processObject(RenderPipeline* pipeline){ //On render pipeline wish to work with object
    //Obtain EditWindow pointer to check if scene is running
    //EditWindow* editwin_ptr = static_cast<EditWindow*>(pipeline->win_ptr);
    if(active == false || alive == false) return; //if object is inactive, not to render it

    TransformProperty* transform_prop = static_cast<TransformProperty*>(this->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
    //Call update on every property in objects
    //if(editwin_ptr->isSceneRun && pipeline->current_state == PIPELINE_STATE_DEFAULT)
        //this->onUpdate(static_cast<int>(pipeline->deltaTime));

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
    //this->onPreRender(pipeline);

    //ZSPIRE::Shader* shader = pipeline->processShaderOnObject(static_cast<void*>(this)); //Will be used next time
    TransformProperty* transform_prop = static_cast<TransformProperty*>(this->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));

    //if(shader != nullptr && transform_prop != nullptr){
        //send transform matrix to shader
        //shader->setTransform(transform_prop->transform_mat);
        //Get mesh pointer
        MeshProperty* mesh_prop = static_cast<MeshProperty*>(this->getPropertyPtrByType(GO_PROPERTY_TYPE_MESH));
        if(mesh_prop != nullptr){
            if(mesh_prop->mesh_ptr != nullptr){
                mesh_prop->mesh_ptr->mesh_ptr->Draw();

            }
        }
    //}
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
