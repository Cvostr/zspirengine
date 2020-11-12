#pragma once

#include "Math.hpp"
#include "../../headers/math/BB3.hpp"
#include <string>
#include <GL/glew.h>
#include <vector>

#define NO_INDICES 0
#define MAX_MESH_BONES 200
#define INSTANCED_RENDER_BUFFER_SIZE 1000

namespace Engine {

    class Bone{
    public:
        std::string bone_name;
        Mat4 offset;

        unsigned int vertices_affected;

        Bone(std::string& name, unsigned int vertices);
        Bone(std::string& name);
    };

    class Mesh {
    public:
        bool alive; //Non destroyed
        std::string mesh_label;

        int mVerticesNum;
        int mIndicesNum;

        //Used to store vertex data (in editor only)
        ZSVERTEX* vertices_arr;
        //Used to store vertex coordinates for bullet physics
        float* vertices_coord;
        //Used to store index data
        unsigned int* indices_arr;

        std::vector<Bone> bones;
        bool hasBones();

        BoundingBox3 mBoundingBox;

        virtual void Init(){}
        virtual void setMeshData(ZSVERTEX* vertices, unsigned int* indices, unsigned int vertices_num, unsigned int indices_num){}
        virtual void setMeshData(ZSVERTEX* vertices, unsigned int vertices_num){}
        virtual void Draw(){}
        virtual void DrawInstanced(unsigned int instances){}
        virtual void DrawLines();
        virtual void Destroy(){}

        Mesh() : mVerticesNum(0), mIndicesNum(0){}
        virtual ~Mesh();
    };

    void setupDefaultMeshes();
    void freeDefaultMeshes();

    Engine::Mesh* allocateMesh();
    Engine::Mesh* allocateMesh(unsigned int size);

    void processTangentSpace(ZSVERTEX* vert_array, unsigned int* indices_array, int indices_num);
    void processTangentSpace(ZSVERTEX* vert_array, int vertices_num);

    Mesh* getPlaneMesh2D();
    Mesh* getUiSpriteMesh2D();
    Mesh* getIsoTileMesh2D();
    Mesh* getCubeMesh3D();
    Mesh* getSphereMesh();
    Mesh* getSkyboxMesh();
    Mesh* getGrassMesh();
}
