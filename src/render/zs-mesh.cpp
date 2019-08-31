#include "../../headers/render/zs-mesh.h"
#include "../../headers/game.h"

#include "../../headers/ogl/ogl.h"
#include "../../headers/vulkan/vk_data.h"

static Engine::Mesh* picked_mesh = nullptr;
extern ZSpireEngine* engine_ptr;

static ZSVERTEX plane_verts[] = {
    // positions              // texture coords
    ZSVERTEX(ZSVECTOR3(1.0f,  1.0f, 0.0f),		ZSVECTOR2(1.0f, 1.0f)),   // top right
    ZSVERTEX(ZSVECTOR3(1.0f, -1.0f, 0.0f),		ZSVECTOR2(1.0f, 0.0f)),   // bottom right
    ZSVERTEX(ZSVECTOR3(-1.0f, -1.0f, 0.0f),		ZSVECTOR2(0.0f, 0.0f)),   // bottom left
    ZSVERTEX(ZSVECTOR3(-1.0f,  1.0f, 0.0f),		ZSVECTOR2(0.0f, 1.0f))   // top left
};

static ZSVERTEX ui_sprite_vertices[] = {
    // positions // texture coords
    ZSVERTEX(ZSVECTOR3(1.0f, 1.0f, 0.0f), ZSVECTOR2(1.0f, 0.0f)), // top right
    ZSVERTEX(ZSVECTOR3(1.0f, 0.0f, 0.0f), ZSVECTOR2(1.0f, 1.0f)), // bottom right
    ZSVERTEX(ZSVECTOR3(0.0f, 0.0f, 0.0f), ZSVECTOR2(0.0f, 1.0f)), // bottom left
    ZSVERTEX(ZSVECTOR3(0.0f, 1.0f, 0.0f), ZSVECTOR2(0.0f, 0.0f)) // top left
};

static ZSVERTEX iso_tile_vertices[] = {
    // positions              // texture coords
    ZSVERTEX(ZSVECTOR3(0.0f,  1.0f, 0.0f),		ZSVECTOR2(0.5f, 1.0f)),
    ZSVERTEX(ZSVECTOR3(1.0f, 0.1f, 0.0f),		ZSVECTOR2(1.0f, 0.55f)),
    ZSVERTEX(ZSVECTOR3(1.0f, -0.1f, 0.0f),		ZSVECTOR2(1.0f, 0.45f)),
    ZSVERTEX(ZSVECTOR3(0.0f,  -1.0f, 0.0f),		ZSVECTOR2(0.5f, 0.0f)),
    ZSVERTEX(ZSVECTOR3(-1.0f,  -0.1f, 0.0f),	ZSVECTOR2(0.0f, 0.45f)),
    ZSVERTEX(ZSVECTOR3(-1.0f,  0.1f, 0.0f),	ZSVECTOR2(0.0f, 0.55f))   // top left
};

static unsigned int plane_inds[] = { 0,1,2, 0,2,3 };
static unsigned int isotile_ind[] = { 0,1,2, 2,3,4, 2,4,5, 2,5,0 };

static Engine::Mesh* plane2Dmesh;
static Engine::Mesh* uiSprite2Dmesh;
static Engine::Mesh* iso_tile2Dmesh;
static Engine::Mesh* cube3Dmesh;

void Engine::setupDefaultMeshes() {
    plane2Dmesh = allocateMesh();
    uiSprite2Dmesh = allocateMesh();
    iso_tile2Dmesh = allocateMesh();
    cube3Dmesh = allocateMesh();

    picked_mesh = nullptr;

    plane2Dmesh->Init(); //Initialize mesh for GL
    plane2Dmesh->setMeshData(plane_verts, plane_inds, 4, 6); //Send plane data

    uiSprite2Dmesh->Init();
    uiSprite2Dmesh->setMeshData(ui_sprite_vertices, plane_inds, 4, 6);

    iso_tile2Dmesh->Init();
    iso_tile2Dmesh->setMeshData(iso_tile_vertices, isotile_ind, 6, 12);

    cube3Dmesh->Init();
    //cube3Dmesh.setMeshData(cube_vertices, 36);
}

void Engine::freeDefaultMeshes(){
    plane2Dmesh->Destroy();
    uiSprite2Dmesh->Destroy();
    iso_tile2Dmesh->Destroy();
    cube3Dmesh->Destroy();
}

Engine::Mesh* Engine::getPlaneMesh2D() {
    return plane2Dmesh;
}

Engine::Mesh* Engine::getUiSpriteMesh2D() {
    return uiSprite2Dmesh;
}

Engine::Mesh* Engine::getIsoTileMesh2D(){
    return iso_tile2Dmesh;
}
Engine::Mesh* Engine::getCubeMesh3D(){
    return cube3Dmesh;
}

void Engine::Mesh::Init(){
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL32 : {
            static_cast<_ogl_Mesh*>(this)->Init();
            break;
        }
        case VULKAN : {
            static_cast<_vk_Mesh*>(this)->Init();
            break;
        }
    }
}

void Engine::Mesh::setMeshData(ZSVERTEX* vertices, unsigned int* indices, unsigned int vertices_num, unsigned int indices_num) {
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL32 : {
            static_cast<_ogl_Mesh*>(this)->setMeshData(vertices, indices, vertices_num, indices_num);
            break;
        }
        case VULKAN : {
            static_cast<_vk_Mesh*>(this)->setMeshData(vertices, indices, vertices_num, indices_num);
            break;
        }
    }
}
void Engine::Mesh::setMeshData(ZSVERTEX* vertices, unsigned int vertices_num) {
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL32 : {
            static_cast<_ogl_Mesh*>(this)->setMeshData(vertices, vertices_num);
            break;
        }
        case VULKAN : {
            static_cast<_vk_Mesh*>(this)->setMeshData(vertices, vertices_num);
            break;
        }
    }
}

void Engine::Mesh::Draw(){
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL32 : {
            static_cast<_ogl_Mesh*>(this)->Draw();
            break;
        }
        case VULKAN : {
            static_cast<_vk_Mesh*>(this)->Draw();
            break;
        }
    }
}

void Engine::Mesh::Destroy(){
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL32 : {
            static_cast<_ogl_Mesh*>(this)->Destroy();
            break;
        }
        case VULKAN : {
            static_cast<_vk_Mesh*>(this)->Destroy();
            break;
        }
    }
}

Engine::Mesh::Mesh(){

}

Engine::Mesh::~Mesh(){

}
