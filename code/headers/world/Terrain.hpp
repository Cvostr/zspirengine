#pragma once

#define TERRAIN_TEXTURES_AMOUNT 12

#include "../render/Mesh.hpp"
#include "../render/Texture.h"
#include "../render/Math.hpp"
#include "../engine/Resources.hpp"

#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>

typedef struct HeightmapGrass{
    std::string diffuse_relpath;
    Engine::TextureResource* diffuse;

    Vec2 scale;
    //Vector to store intanced transforms
    std::vector<Mat4> inst_transform;

    HeightmapGrass(){
        diffuse_relpath = "@none";
        diffuse = nullptr;

        scale = Vec2(2.f, 2.f);
    }

}HeightmapGrass;

typedef struct HeightmapTexel{
    float height;
    unsigned char texture_factors[TERRAIN_TEXTURES_AMOUNT];
    int grass;
    bool modified;

    HeightmapTexel(){
        height = 0;
        grass = 0;
        modified = true;
    }
}HeightmapTexel;

typedef struct HeightmapVertex{
    Vec3 pos;
    Vec2 uv;
    Vec3 normal;
    Vec3 tangent;
    Vec3 bitangent;
}HeightmapVertex;

typedef struct HeightmapTexturePair{
    std::string diffuse_relpath;
    std::string normal_relpath;

    Engine::TextureResource* diffuse;
    Engine::TextureResource* normal;

    HeightmapTexturePair(){
        diffuse_relpath = "@none";
        normal_relpath = "@none";
        diffuse = nullptr;
        normal = nullptr;
    }
}HeightmapTexturePair;

typedef struct TerrainPainting{
    //OpenGL texture IDs of texture masks
    Engine::Texture* texture_mask1;
    Engine::Texture* texture_mask2;
    Engine::Texture* texture_mask3;
    //Memory for paintings
    unsigned char* _texture;
    unsigned char* _texture1;
    unsigned char* _texture2;

    TerrainPainting() {
        _texture = nullptr;
        _texture1 = nullptr;
        _texture2 = nullptr;

        texture_mask1 = Engine::allocTexture();
        texture_mask2 = Engine::allocTexture();
        texture_mask3 = Engine::allocTexture();
    }

    void freeMem() {
        delete[] _texture;
        delete[] _texture1;
        delete[] _texture2;
    }
}TerrainPainting;

class TerrainData{
private:
    bool created;
    //Stores IDs for paintings and memory
    TerrainPainting painting;
    //Stores Opengl mesh variables
    Engine::Mesh* meshGL;
    //Array for vertex data to GL
    HeightmapVertex* vertices;
    //Array for indices
    unsigned int* indices;

public:
    //terrain sizes
    int W, H;

    bool hasHeightmapChanged;
    bool hasPaintingChanged;
    bool hasGrassChanged;
    bool hasPhysicShapeChanged;
    //Pointer to object of bullet physics shape
    btBvhTriangleMeshShape* shape;
    //Data for each vertex of heightmap
    HeightmapTexel* data;
    //Stores all grass types
    std::vector<HeightmapGrass> grass;
    float GrassDensity;
    //Allocate arrays
    void alloc(int W, int H);
    //Make terrain absolutely flat with specified height
    void flatTerrain(float height);
    //Init opengl varibles
    void initGL();
    void initPhysics();
    //Destroy GL variables
    void destroyGL();
    void Draw(bool picking = false, unsigned int instances = 1);
    void generateGLMesh();
    void processNormalsTangentSpace(HeightmapVertex* vert_array, unsigned int* indices_array, int indices_num);
    void saveToFile(const char* file_path);
    bool loadFromFile(const char* file_path);
    bool loadFromMemory(const char* bytes);

    void modifyHeight(int originX, int originY, float originHeight, int range, int multiplyer);
    void modifyTexture(int originX, int originY, int range, unsigned char texture);
    void plantGrass(int originX, int originY, int range, int grass);

    void sum(unsigned char* ptr, int val);
    void reduce(unsigned char* ptr, int val);
    bool isCreated();

    void copyTo(TerrainData* dest);

    void updateTextureBuffers(bool full_rebuild = true);
    void updateTextureBuffersGL();

    void updateGeometryBuffers(bool full_rebuild = true);
    void updateGeometryBuffersGL();

    void updateGrassBuffers();

    TerrainData();
    ~TerrainData();
};
