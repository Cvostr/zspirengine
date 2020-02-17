#ifndef TERRAIN_H
#define TERRAIN_H

#define TERRAIN_TEXTURES_AMOUNT 12

#include "../render/zs-texture.h"
#include "../render/zs-math.h"
#include "../engine/resources.h"

#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>

typedef struct HeightmapGrass{
    std::string diffuse_relpath;
    Engine::TextureResource* diffuse;

    ZSVECTOR2 scale;

    HeightmapGrass(){
        diffuse_relpath = "@none";
        diffuse = nullptr;

        scale = ZSVECTOR2(2.f, 2.f);
    }

}HeightmapGrass;

typedef struct HeightmapTexel{
    float height;
    unsigned char texture_factors[TERRAIN_TEXTURES_AMOUNT];
    int grass;

    HeightmapTexel(){
        grass = 0;
    }
}HeightmapTexel;

typedef struct HeightmapVertex{
    ZSVECTOR3 pos;
    ZSVECTOR2 uv;
    ZSVECTOR3 normal;
    ZSVECTOR3 tangent;
    ZSVECTOR3 bitangent;
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
    unsigned int texture_mask1;
    unsigned int texture_mask2;
    unsigned int texture_mask3;
    //Memory for paintings
    unsigned char* _texture;
    unsigned char* _texture1;
    unsigned char* _texture2;
}TerrainPainting;

class TerrainData{
private:
    bool created;
    //Stores IDs for paintings and memory
    TerrainPainting painting;

    HeightmapVertex* vertices;
    unsigned int* indices;

public:
    int W, H;

    bool hasHeightmapChanged;
    bool hasPaintingChanged;
    bool hasGrassChanged;
    bool hasPhysicShapeChanged;

    btBvhTriangleMeshShape* shape;

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    HeightmapTexel* data;


    void alloc(int W, int H);
    void flatTerrain(int height);

    void initGL();
    void initPhysics();
    void destroyGL();
    void Draw(bool picking = false);
    void generateGLMesh();
    void processTangentSpace(HeightmapVertex* vert_array, unsigned int* indices_array, int indices_num);
    void saveToFile(const char* file_path);
    bool loadFromFile(const char* file_path);

    void modifyHeight(int originX, int originY, float originHeight, int range, int multiplyer);
    void modifyTexture(int originX, int originY, int range, unsigned char texture);
    void plantGrass(int originX, int originY, int range, int grass);

    void sum(unsigned char* ptr, int val);
    void reduce(unsigned char* ptr, int val);

    void copyTo(TerrainData* dest);

    void updateTextureBuffers();
    void updateTextureBuffersGL();

    void updateGeometryBuffers();
    void updateGeometryBuffersGL();

    TerrainData();
    ~TerrainData();
};

#endif // TERRAIN_H
