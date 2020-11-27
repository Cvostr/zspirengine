#include "../../headers/ogl/GLRenderer.hpp"
#include "../../headers/game.h"

extern ZSpireEngine* engine_ptr;
extern ZSGAME_DATA* game_data;

Engine::GLRenderer::GLRenderer() {
    InitShaders();
}

void Engine::GLRenderer::InitShaders() {
    this->ui_shader->compileFromFile("Shaders/ui/ui.vert", "Shaders/ui/ui.frag");

    if (engine_ptr->desc->game_perspective == PERSP_2D) {
        this->tile_shader->compileFromFile("Shaders/2d_tile/tile2d.vert", "Shaders/2d_tile/tile2d.frag");
    }
    if (engine_ptr->desc->game_perspective == PERSP_3D) {

        this->deffered_light->compileFromFile("Shaders/postprocess/deffered_light/deffered.vert", "Shaders/postprocess/deffered_light/deffered.frag");
        this->default3d->compileFromFile("Shaders/3d/3d.vert", "Shaders/3d/3d.frag");
        this->skybox_shader->compileFromFile("Shaders/skybox/skybox.vert", "Shaders/skybox/skybox.frag");
        this->terrain_shader->compileFromFile("Shaders/heightmap/heightmap.vert", "Shaders/heightmap/heightmap.frag");
        this->grass_shader->compileFromFile("Shaders/heightmap/grass.vert", "Shaders/heightmap/grass.frag");
        this->shadowMap->compileFromFile("Shaders/shadowmap/shadowmap.vert", "Shaders/shadowmap/shadowmap.frag", "Shaders/shadowmap/shadowmap.geom");
        this->final_shader->compileFromFile("Shaders/postprocess/final/final.vert", "Shaders/postprocess/final/final.frag");
        this->water_shader->compileFromFile("Shaders/water/water.vert", "Shaders/water/water.frag");

    }
}

void Engine::GLRenderer::initManager() {
    setDepthState(true);

    if (this->game_desc_ptr->game_perspective == PERSP_2D) {
        cullFaces = false;
        setFaceCullState(false);
    }
    else {
        cullFaces = true;
        setFaceCullState(true);
    }
    //if we use opengl, then create GBUFFER in GL commands
    if (this->game_desc_ptr->game_perspective == PERSP_3D) {
        create_G_Buffer(this->WIDTH, this->HEIGHT);
    }
}


void Engine::GLRenderer::render2D() {
    World* world_ptr = game_data->world;

    if (engine_ptr->engine_info->graphicsApi == OGL32) {
        setClearColor(0, 0, 0, 1);
        ClearFBufferGL(true, true);
        glEnable(GL_BLEND); //Disable blending to render Skybox and shadows
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        setFullscreenViewport(this->WIDTH, this->HEIGHT);
        setDepthState(true);
        //Render objects
        processObjects(world_ptr);
    }
}

void Engine::GLRenderer::render3D(Engine::Camera* cam) {
    World* world_ptr = game_data->world;
    //Render shadows, first
    TryRenderShadows(cam);
    this->cam = cam;
    {
        //Bind Geometry Buffer to make Deferred Shading
        gbuffer->bind();
        setClearColor(0, 0, 0, 1);
        ClearFBufferGL(true, true);
        {
            //Render Skybox
            setBlendingState(false);
            setFullscreenViewport(this->WIDTH, this->HEIGHT);
            TryRenderSkybox();
        }
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
        df_light_buffer->bind();
        ClearFBufferGL(true, false); //Clear screen
        //Disable depth rendering to draw plane correctly
        setDepthState(false);
        setFaceCullState(false);
        gbuffer->bindTextures(10); //Bind gBuffer textures
        deffered_light->Use(); //use deffered shader
        Engine::getPlaneMesh2D()->Draw(); //Draw screen
    }

    //Render ALL UI
    {
        ui_buffer->bind();
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
        df_light_buffer->bindTextures(0);
        ui_buffer->bindTextures(1);
        final_shader->Use();
        Engine::getPlaneMesh2D()->Draw(); //Draw screen
    }
}