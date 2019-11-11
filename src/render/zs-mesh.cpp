#include "../../headers/render/zs-mesh.h"
#include "../../headers/game.h"

#include "../../headers/ogl/ogl.h"
#include "../../headers/vulkan/vk_data.h"

#define SPHERE_SECTORS 36
#define SPHERE_STACKS 18

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

static ZSVERTEX cube_vertices[] = {
    // back face
    ZSVERTEX(ZSVECTOR3(-1.0f, -1.0f, -1.0f), ZSVECTOR2(0.0f, 0.0f), ZSVECTOR3(0.0f,  0.0f, -1.0f)), // bottom-left
    ZSVERTEX(ZSVECTOR3(1.0f,  1.0f, -1.0f), ZSVECTOR2(1.0f, 1.0f), ZSVECTOR3(0.0f,  0.0f, -1.0f)), // top-right
    ZSVERTEX(ZSVECTOR3(1.0f, -1.0f, -1.0f), ZSVECTOR2(1.0f, 0.0f), ZSVECTOR3(0.0f,  0.0f, -1.0f)), // bottom-right
    ZSVERTEX(ZSVECTOR3(1.0f,  1.0f, -1.0f), ZSVECTOR2(1.0f, 1.0f), ZSVECTOR3(0.0f,  0.0f, -1.0f)), // top-right
    ZSVERTEX(ZSVECTOR3(-1.0f, -1.0f, -1.0f), ZSVECTOR2(0.0f, 0.0f), ZSVECTOR3(0.0f,  0.0f, -1.0f)), // bottom-left
    ZSVERTEX(ZSVECTOR3(-1.0f,  1.0f, -1.0f), ZSVECTOR2(0.0f, 1.0f), ZSVECTOR3(0.0f,  0.0f, -1.0f)), // top-left
    // front face
    ZSVERTEX(ZSVECTOR3(-1.0f, -1.0f,  1.0f), ZSVECTOR2(0.0f, 0.0f), ZSVECTOR3(0.0f,  0.0f,  1.0f)), // bottom-left
    ZSVERTEX(ZSVECTOR3(1.0f, -1.0f,  1.0f), ZSVECTOR2(1.0f, 0.0f), ZSVECTOR3(0.0f,  0.0f,  1.0f)), // bottom-right
    ZSVERTEX(ZSVECTOR3(1.0f,  1.0f,  1.0f), ZSVECTOR2(1.0f, 1.0f), ZSVECTOR3(0.0f,  0.0f,  1.0f)), // top-right
    ZSVERTEX(ZSVECTOR3(1.0f,  1.0f,  1.0f), ZSVECTOR2(1.0f, 1.0f), ZSVECTOR3(0.0f,  0.0f,  1.0f)), // top-right
    ZSVERTEX(ZSVECTOR3(-1.0f,  1.0f,  1.0f), ZSVECTOR2(0.0f, 1.0f), ZSVECTOR3(0.0f,  0.0f,  1.0f)), // top-left
    ZSVERTEX(ZSVECTOR3(-1.0f, -1.0f,  1.0f), ZSVECTOR2(0.0f, 0.0f), ZSVECTOR3(0.0f,  0.0f,  1.0f)), // bottom-left
    // left face
    ZSVERTEX(ZSVECTOR3(-1.0f,  1.0f,  1.0f), ZSVECTOR2(1.0f, 0.0f), ZSVECTOR3(-1.0f,  0.0f,  0.0f)), // top-right
    ZSVERTEX(ZSVECTOR3(-1.0f,  1.0f, -1.0f), ZSVECTOR2(1.0f, 1.0f), ZSVECTOR3(-1.0f,  0.0f,  0.0f)), // top-left
    ZSVERTEX(ZSVECTOR3(-1.0f, -1.0f, -1.0f), ZSVECTOR2(0.0f, 1.0f), ZSVECTOR3(-1.0f,  0.0f,  0.0f)), // bottom-left
    ZSVERTEX(ZSVECTOR3(-1.0f, -1.0f, -1.0f), ZSVECTOR2(0.0f, 1.0f), ZSVECTOR3(-1.0f,  0.0f,  0.0f)), // bottom-left
    ZSVERTEX(ZSVECTOR3(-1.0f, -1.0f,  1.0f), ZSVECTOR2(0.0f, 0.0f), ZSVECTOR3(-1.0f,  0.0f,  0.0f)), // bottom-right
    ZSVERTEX(ZSVECTOR3(-1.0f,  1.0f,  1.0f), ZSVECTOR2(1.0f, 0.0f), ZSVECTOR3(-1.0f,  0.0f,  0.0f)), // top-right
    // right face
    ZSVERTEX(ZSVECTOR3(1.0f,  1.0f,  1.0f), ZSVECTOR2(1.0f, 0.0f), ZSVECTOR3(1.0f,  0.0f,  0.0f)), // top-left
    ZSVERTEX(ZSVECTOR3(1.0f, -1.0f, -1.0f), ZSVECTOR2(0.0f, 1.0f), ZSVECTOR3(1.0f,  0.0f,  0.0f)), // bottom-right
    ZSVERTEX(ZSVECTOR3(1.0f,  1.0f, -1.0f), ZSVECTOR2(1.0f, 1.0f), ZSVECTOR3(1.0f,  0.0f,  0.0f)), // top-right
    ZSVERTEX(ZSVECTOR3(1.0f, -1.0f, -1.0f), ZSVECTOR2(0.0f, 1.0f), ZSVECTOR3(1.0f,  0.0f,  0.0f)), // bottom-right
    ZSVERTEX(ZSVECTOR3(1.0f,  1.0f,  1.0f), ZSVECTOR2(1.0f, 0.0f), ZSVECTOR3(1.0f,  0.0f,  0.0f)), // top-left
    ZSVERTEX(ZSVECTOR3(1.0f, -1.0f,  1.0f), ZSVECTOR2(0.0f, 0.0f), ZSVECTOR3(1.0f,  0.0f,  0.0f)), // bottom-left
    // bottom face
    ZSVERTEX(ZSVECTOR3(-1.0f, -1.0f, -1.0f), ZSVECTOR2(0.0f, 1.0f), ZSVECTOR3(0.0f, -1.0f,  0.0f)), // top-right
    ZSVERTEX(ZSVECTOR3(1.0f, -1.0f, -1.0f), ZSVECTOR2(1.0f, 1.0f), ZSVECTOR3(0.0f, -1.0f,  0.0f)), // top-left
    ZSVERTEX(ZSVECTOR3(1.0f, -1.0f,  1.0f), ZSVECTOR2(1.0f, 0.0f), ZSVECTOR3(0.0f, -1.0f,  0.0f)), // bottom-left
    ZSVERTEX(ZSVECTOR3(1.0f, -1.0f,  1.0f), ZSVECTOR2(1.0f, 0.0f), ZSVECTOR3(0.0f, -1.0f,  0.0f)), // bottom-left
    ZSVERTEX(ZSVECTOR3(-1.0f, -1.0f,  1.0f), ZSVECTOR2(0.0f, 0.0f), ZSVECTOR3( 0.0f, -1.0f,  0.0f)), // bottom-right
    ZSVERTEX(ZSVECTOR3(-1.0f, -1.0f, -1.0f), ZSVECTOR2(0.0f, 1.0f), ZSVECTOR3(0.0f, -1.0f,  0.0f)), // top-right
    // top face
    ZSVERTEX(ZSVECTOR3(-1.0f,  1.0f, -1.0f), ZSVECTOR2(0.0f, 1.0f), ZSVECTOR3(0.0f,  1.0f,  0.0f)), // top-left
    ZSVERTEX(ZSVECTOR3(1.0f,  1.0f , 1.0f), ZSVECTOR2(1.0f, 0.0f), ZSVECTOR3(0.0f,  1.0f,  0.0f)), // bottom-right
    ZSVERTEX(ZSVECTOR3(1.0f,  1.0f, -1.0f), ZSVECTOR2(1.0f, 1.0f), ZSVECTOR3(0.0f,  1.0f,  0.0f)), // top-right
    ZSVERTEX(ZSVECTOR3(1.0f,  1.0f,  1.0f), ZSVECTOR2(1.0f, 0.0f), ZSVECTOR3(0.0f,  1.0f,  0.0f)), // bottom-right
    ZSVERTEX(ZSVECTOR3(-1.0f,  1.0f, -1.0f), ZSVECTOR2(0.0f, 1.0f), ZSVECTOR3(0.0f,  1.0f,  0.0f)), // top-left
    ZSVERTEX(ZSVECTOR3(-1.0f,  1.0f,  1.0f), ZSVECTOR2(0.0f, 0.0f), ZSVECTOR3(0.0f,  1.0f,  0.0f))  // bottom-left
};

static ZSVERTEX skyboxVertices[] = {
    // positions
    ZSVERTEX(ZSVECTOR3(-1.0f,  1.0f, -1.0f)),
    ZSVERTEX(ZSVECTOR3(-1.0f, -1.0f, -1.0f)),
    ZSVERTEX(ZSVECTOR3(1.0f, -1.0f, -1.0f)),
    ZSVERTEX(ZSVECTOR3(1.0f, -1.0f, -1.0f)),
    ZSVERTEX(ZSVECTOR3(1.0f,  1.0f, -1.0f)),
    ZSVERTEX(ZSVECTOR3(-1.0f,  1.0f, -1.0f)),

    ZSVERTEX(ZSVECTOR3(-1.0f, -1.0f,  1.0f)),
    ZSVERTEX(ZSVECTOR3(-1.0f, -1.0f, -1.0f)),
    ZSVERTEX(ZSVECTOR3(-1.0f,  1.0f, -1.0f)),
    ZSVERTEX(ZSVECTOR3(-1.0f,  1.0f, -1.0f)),
    ZSVERTEX(ZSVECTOR3(-1.0f,  1.0f,  1.0f)),
    ZSVERTEX(ZSVECTOR3(-1.0f, -1.0f,  1.0f)),

    ZSVERTEX(ZSVECTOR3(1.0f, -1.0f, -1.0f)),
    ZSVERTEX(ZSVECTOR3(1.0f, -1.0f,  1.0f)),
    ZSVERTEX(ZSVECTOR3(1.0f,  1.0f,  1.0f)),
    ZSVERTEX(ZSVECTOR3(1.0f,  1.0f,  1.0f)),
    ZSVERTEX(ZSVECTOR3(1.0f,  1.0f, -1.0f)),
    ZSVERTEX(ZSVECTOR3(1.0f, -1.0f, -1.0f)),

    ZSVERTEX(ZSVECTOR3(-1.0f, -1.0f,  1.0f)),
    ZSVERTEX(ZSVECTOR3(-1.0f,  1.0f,  1.0f)),
    ZSVERTEX(ZSVECTOR3(1.0f,  1.0f,  1.0f)),
    ZSVERTEX(ZSVECTOR3(1.0f,  1.0f,  1.0f)),
    ZSVERTEX(ZSVECTOR3(1.0f, -1.0f,  1.0f)),
    ZSVERTEX(ZSVECTOR3(-1.0f, -1.0f,  1.0f)),

    ZSVERTEX(ZSVECTOR3(-1.0f,  1.0f, -1.0f)),
    ZSVERTEX(ZSVECTOR3(1.0f,  1.0f, -1.0f)),
    ZSVERTEX(ZSVECTOR3(1.0f,  1.0f,  1.0f)),
    ZSVERTEX(ZSVECTOR3(1.0f,  1.0f,  1.0f)),
    ZSVERTEX(ZSVECTOR3(-1.0f,  1.0f,  1.0f)),
    ZSVERTEX(ZSVECTOR3(-1.0f,  1.0f, -1.0f)),

    ZSVERTEX(ZSVECTOR3(-1.0f, -1.0f, -1.0f)),
    ZSVERTEX(ZSVECTOR3(-1.0f, -1.0f,  1.0f)),
    ZSVERTEX(ZSVECTOR3(1.0f, -1.0f, -1.0f)),
    ZSVERTEX(ZSVECTOR3(1.0f, -1.0f, -1.0f)),
    ZSVERTEX(ZSVECTOR3(-1.0f, -1.0f,  1.0f)),
    ZSVERTEX(ZSVECTOR3(1.0f, -1.0f,  1.0f))
};


static ZSVERTEX grass_vertices[] = {
    // back face
    ZSVERTEX(ZSVECTOR3(-0.5f, 0.0f, 0.0f), ZSVECTOR2(0.0f, 0.0f), ZSVECTOR3(0.0f,  0.0f, -1.0f)), // bottom-left
    ZSVERTEX(ZSVECTOR3(0.5f, 0.0f, 0.0f), ZSVECTOR2(1.0f, 1.0f), ZSVECTOR3(0.0f,  0.0f, -1.0f)), // top-right
    ZSVERTEX(ZSVECTOR3(0.5f, 0.5f, 0.0f), ZSVECTOR2(1.0f, 0.0f), ZSVECTOR3(0.0f,  0.0f, -1.0f)), // bottom-right
    ZSVERTEX(ZSVECTOR3(-0.5f, 0.5f, 0.0f), ZSVECTOR2(1.0f, 1.0f), ZSVECTOR3(0.0f,  0.0f, -1.0f)), // top-right

    ZSVERTEX(ZSVECTOR3(0.0f, 0.0f, 0.5f), ZSVECTOR2(0.0f, 0.0f), ZSVECTOR3(0.0f,  0.0f, -1.0f)), // bottom-left
    ZSVERTEX(ZSVECTOR3(0.0f, 0.0f, -0.5f), ZSVECTOR2(0.0f, 1.0f), ZSVECTOR3(0.0f,  0.0f, -1.0f)), // top-left
    ZSVERTEX(ZSVECTOR3(0.0f, 0.5f, 0.5f), ZSVECTOR2(0.0f, 0.0f), ZSVECTOR3(0.0f,  0.0f,  1.0f)), // bottom-left
    ZSVERTEX(ZSVECTOR3(0.0f, 0.5f, -0.5f), ZSVECTOR2(1.0f, 0.0f), ZSVECTOR3(0.0f,  0.0f,  1.0f)), // bottom-right

};

static unsigned int plane_inds[] = { 0,1,2, 0,2,3 };
static unsigned int isotile_ind[] = { 0,1,2, 2,3,4, 2,4,5, 2,5,0 };
static unsigned int grass_ind[] = {0,1,2,2,1,0,  0,2,3,3,2,0,   4,5,7,7,5,4,  4,7,6,6,4,7};

static Engine::Mesh* plane2Dmesh;
static Engine::Mesh* uiSprite2Dmesh;
static Engine::Mesh* iso_tile2Dmesh;
static Engine::Mesh* cube3Dmesh;
static Engine::Mesh* skyboxMesh;
static Engine::Mesh* sphereMesh;
static Engine::Mesh* grassMesh;

void Engine::setupDefaultMeshes() {
    plane2Dmesh = allocateMesh();
    uiSprite2Dmesh = allocateMesh();
    iso_tile2Dmesh = allocateMesh();
    cube3Dmesh = allocateMesh();
    skyboxMesh = allocateMesh();
    sphereMesh = allocateMesh();
    grassMesh = allocateMesh();

    picked_mesh = nullptr;

    plane2Dmesh->Init(); //Initialize mesh for GL
    plane2Dmesh->setMeshData(plane_verts, plane_inds, 4, 6); //Send plane data

    uiSprite2Dmesh->Init();
    uiSprite2Dmesh->setMeshData(ui_sprite_vertices, plane_inds, 4, 6);

    iso_tile2Dmesh->Init();
    iso_tile2Dmesh->setMeshData(iso_tile_vertices, isotile_ind, 6, 12);

    cube3Dmesh->Init();
    processTangentSpace(&cube_vertices[0], 36);
    cube3Dmesh->setMeshData(&cube_vertices[0], 36);

    sphereMesh->Init();
    std::vector<ZSVERTEX> sphere_v;
    std::vector<unsigned int> sphere_indices;
    float radius = 1.f;
    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
    float s, t;                                     // vertex texCoord

    float sectorStep = 2 * ZS_PI / SPHERE_SECTORS;
    float stackStep = ZS_PI / SPHERE_STACKS;
    float sectorAngle, stackAngle;

    for(int i = 0; i <= SPHERE_STACKS; ++i)
    {
        stackAngle = ZS_PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for(int j = 0; j <= SPHERE_SECTORS; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi
            ZSVERTEX v;

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            v.pos = ZSVECTOR3(x, y, z);

            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            v.normal = ZSVECTOR3(nx, ny, nz);

            // vertex tex coord (s, t) range between [0, 1]
            s = static_cast<float>(j) / SPHERE_SECTORS;
            t = static_cast<float>(i) / SPHERE_STACKS;
            v.uv = ZSVECTOR2(s, t);
            sphere_v.push_back(v);
        }
    }
    unsigned int k1, k2;
    for(unsigned int i = 0; i < SPHERE_STACKS; ++i)
    {
        k1 = i * (SPHERE_SECTORS + 1);     // beginning of current stack
        k2 = k1 + SPHERE_SECTORS + 1;      // beginning of next stack

        for(int j = 0; j < SPHERE_SECTORS; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if(i != 0)
            {
                sphere_indices.push_back(k1);
                sphere_indices.push_back(k2);
                sphere_indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if(i != (SPHERE_STACKS-1))
            {
                sphere_indices.push_back(k1 + 1);
                sphere_indices.push_back(k2);
                sphere_indices.push_back(k2 + 1);
            }
        }
    }

    processTangentSpace(&sphere_v[0], sphere_indices.data(), static_cast<int>(sphere_indices.size()));
    sphereMesh->setMeshData(sphere_v.data(), sphere_indices.data(), static_cast<uint>(sphere_v.size()), static_cast<uint>(sphere_indices.size())); //Send plane data

    skyboxMesh->Init();
    skyboxMesh->setMeshData(&skyboxVertices[0], 36); //Send plane data

    grassMesh->Init();
    grassMesh->setMeshData(grass_vertices, grass_ind, 8, 24);
}

void Engine::freeDefaultMeshes(){
    plane2Dmesh->Destroy();
    uiSprite2Dmesh->Destroy();
    iso_tile2Dmesh->Destroy();
    cube3Dmesh->Destroy();
    sphereMesh->Destroy();
    skyboxMesh->Destroy();
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
Engine::Mesh* Engine::getSphereMesh(){
    return sphereMesh;
}
Engine::Mesh* Engine::getSkyboxMesh(){
    return skyboxMesh;
}
Engine::Mesh* Engine::getGrassMesh(){
    return grassMesh;

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

void Engine::Mesh::DrawLines(){
    if(engine_ptr->engine_info->graphicsApi == OGL32)
        static_cast<_ogl_Mesh*>(this)->DrawLines();
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

bool Engine::Mesh::hasBones(){
    return !(bones.size() == 0);
}

Engine::Mesh::Mesh(){

}

Engine::Mesh::~Mesh(){
    delete vertices_arr;
    delete vertices_coord;
    delete indices_arr;
}


Engine::Bone::Bone(std::string& name, unsigned int vertices){
    this->bone_name = name;
    this->vertices_affected = vertices;
}
Engine::Bone::Bone(std::string& name){
    this->bone_name = name;
}

void Engine::processTangentSpace(ZSVERTEX* vert_array, unsigned int* indices_array, int indices_num){
    for(int ind_i = 0; ind_i < indices_num ; ind_i += 3){
        ZSVERTEX v1 = vert_array[indices_array[ind_i]];
        ZSVERTEX v2 = vert_array[indices_array[ind_i + 1]];
        ZSVERTEX v3 = vert_array[indices_array[ind_i + 2]];

        ZSVECTOR3 edge1 = v2.pos - v1.pos;
        ZSVECTOR3 edge2 = v3.pos - v1.pos;
        ZSVECTOR2 deltaUV1 = v2.uv - v1.uv;
        ZSVECTOR2 deltaUV2 = v3.uv - v1.uv;

        float f = 1.0f / (deltaUV1.X * deltaUV2.Y - deltaUV2.X * deltaUV1.Y);

        ZSVECTOR3 tangent, bitangent;
        tangent.X = f * (deltaUV2.Y * edge1.X - deltaUV1.Y * edge2.X);
        tangent.Y = f * (deltaUV2.Y * edge1.Y - deltaUV1.Y * edge2.Y);
        tangent.Z = f * (deltaUV2.Y * edge1.Z - deltaUV1.Y * edge2.Z);
        vNormalize(&tangent);

        bitangent.X = f * (-deltaUV2.X * edge1.X + deltaUV1.X * edge2.X);
        bitangent.Y = f * (-deltaUV2.X * edge1.Y + deltaUV1.X * edge2.Y);
        bitangent.Z = f * (-deltaUV2.X * edge1.Z + deltaUV1.X * edge2.Z);
        vNormalize(&bitangent);
        for(int i = 0; i < 3; i ++){
            vert_array[indices_array[ind_i + i]].tangent = tangent;
            vert_array[indices_array[ind_i + i]].bitangent = bitangent;
        }
    }
}

void Engine::processTangentSpace(ZSVERTEX* vert_array, int vertices_num){
    for(int v_i = 0; v_i < vertices_num ; v_i += 3){
        ZSVERTEX v1 = vert_array[v_i];
        ZSVERTEX v2 = vert_array[v_i + 1];
        ZSVERTEX v3 = vert_array[v_i + 2];

        ZSVECTOR3 edge1 = v2.pos - v1.pos;
        ZSVECTOR3 edge2 = v3.pos - v1.pos;
        ZSVECTOR2 deltaUV1 = v2.uv - v1.uv;
        ZSVECTOR2 deltaUV2 = v3.uv - v1.uv;

        float f = 1.0f / (deltaUV1.X * deltaUV2.Y - deltaUV2.X * deltaUV1.Y);

        ZSVECTOR3 tangent, bitangent;
        tangent.X = f * (deltaUV2.Y * edge1.X - deltaUV1.Y * edge2.X);
        tangent.Y = f * (deltaUV2.Y * edge1.Y - deltaUV1.Y * edge2.Y);
        tangent.Z = f * (deltaUV2.Y * edge1.Z - deltaUV1.Y * edge2.Z);
        vNormalize(&tangent);

        bitangent.X = f * (-deltaUV2.X * edge1.X + deltaUV1.X * edge2.X);
        bitangent.Y = f * (-deltaUV2.X * edge1.Y + deltaUV1.X * edge2.Y);
        bitangent.Z = f * (-deltaUV2.X * edge1.Z + deltaUV1.X * edge2.Z);
        vNormalize(&bitangent);

        tangent.Z *= -1;
        bitangent.Z *= -1;

        tangent.X *= -1;
        bitangent.X *= -1;

        for(int i = v_i; i < v_i + 3; i ++){
            vert_array[i].tangent = tangent;
            vert_array[i].bitangent = bitangent;
        }
    }
}
