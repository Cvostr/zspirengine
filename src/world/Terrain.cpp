#include "../../headers/world/terrain.h"
#include <fstream>
#include <iostream>
#include <GL/glew.h>

TerrainData::TerrainData(){
    W = 0; H = 0;
    created = false;
    hasHeightmapChanged = false;
    hasPaintingChanged = false;
    hasPhysicShapeChanged = false;
    shape = nullptr;
    data = nullptr;
}

TerrainData::~TerrainData(){
    if(created)
        delete[] data;

    if(shape)
        delete shape;
}

void TerrainData::alloc(int W, int H){
    if(created)
        //Delete data if already allocated
        delete[] data;
    this->W = W;
    this->H = H;
    //Allocate memory for terrain data
    this->data = new HeightmapTexel[static_cast<unsigned int>(W * H)];
    //Make terrain absolutely flat
    flatTerrain(0);
}

void TerrainData::flatTerrain(int height){
    for(int i = 0; i < W * H; i ++){
        data[i].height = height;
        this->data[i].texture_factors[0] = 255;
        for(int t = 1; t < TERRAIN_TEXTURES_AMOUNT; t++)
            this->data[i].texture_factors[t] = 0;
    }
}

void TerrainData::updateTextureBuffers(){
    //write temporary array to store texture ids
    painting._texture = new unsigned char[static_cast<unsigned int>(W * H * 4)];
    painting._texture1 = new unsigned char[static_cast<unsigned int>(W * H * 4)];
    painting._texture2 = new unsigned char[static_cast<unsigned int>(W * H * 4)];
    for(int y = 0; y < H; y ++){
        for(int x = 0; x < W; x ++){
           painting._texture[(x * H + y) * 4] = data[x * H + y].texture_factors[0];
           painting._texture[(x * H + y) * 4 + 1] = data[x * H + y].texture_factors[1];
           painting._texture[(x * H + y) * 4 + 2] = data[x * H + y].texture_factors[2];
           painting._texture[(x * H + y) * 4 + 3] = data[x * H + y].texture_factors[3];

           painting._texture1[(x * H + y) * 4] = data[x * H + y].texture_factors[4];
           painting._texture1[(x * H + y) * 4 + 1] = data[x * H + y].texture_factors[5];
           painting._texture1[(x * H + y) * 4 + 2] = data[x * H + y].texture_factors[6];
           painting._texture1[(x * H + y) * 4 + 3] = data[x * H + y].texture_factors[7];

           painting._texture2[(x * H + y) * 4] = data[x * H + y].texture_factors[8];
           painting._texture2[(x * H + y) * 4 + 1] = data[x * H + y].texture_factors[9];
           painting._texture2[(x * H + y) * 4 + 2] = data[x * H + y].texture_factors[10];
           painting._texture2[(x * H + y) * 4 + 3] = data[x * H + y].texture_factors[11];
        }
    }
}

void TerrainData::updateGeometryBuffers(){
    //Allocate vertices array
    vertices = new HeightmapVertex[static_cast<unsigned int>(W * H)];
    //Allocate indices array
    indices = new unsigned int[static_cast<unsigned int>((W - 1) * (H - 1) * 2 * 3)];

    //Iterate over all vertices and fill their vectors
    for(int y = 0; y < H; y ++){
        for(int x = 0; x < W; x ++){
            //Set vertex height
            vertices[x * H + y].pos = ZSVECTOR3(x, data[x * H + y].height, y);
            //Calculate vertex texture UV
            vertices[x * H + y].uv = ZSVECTOR2(static_cast<float>(x) / W, static_cast<float>(y) / H);
        }
    }
    //Declare indices iterator
    unsigned int inds = 0;
    //Iterate over all vertices
    for(int y = 0; y < H - 1; y ++){
        for(int x = 0; x < W - 1; x ++){
            indices[inds] = static_cast<unsigned int>(x * H + y);
            indices[inds + 2] = static_cast<unsigned int>(x * H + H + y);
            indices[inds + 1] = static_cast<unsigned int>(x * H + H + y + 1);

            indices[inds + 3] = static_cast<unsigned int>(x * H + y);
            indices[inds + 5] = static_cast<unsigned int>(x * H + H + y + 1);
            indices[inds + 4] = static_cast<unsigned int>(x * H + y + 1);

            inds += 6;
        }
    }

    for(unsigned int i = 0; i < inds - 2; i ++){
        HeightmapVertex* v1 = &vertices[indices[i]];
        HeightmapVertex* v2 = &vertices[indices[i + 1]];
        HeightmapVertex* v3 = &vertices[indices[i + 2]];
        //Get two sides of triangle
        ZSVECTOR3 v12 = v1->pos - v2->pos;
        ZSVECTOR3 v13 = v1->pos - v3->pos;
        //Calculate normal
        v1->normal = vCross(v12, v13);
        //Normalize vector
        vNormalize(&v1->normal);
    }
    //generate tangent, bitangent
    processTangentSpace(vertices, indices, (W - 1) * (H - 1) * 2 * 3);
}

bool TerrainData::loadFromFile(const char* file_path){
    std::ifstream world_stream;
    world_stream.open(file_path, std::ifstream::binary);

    if(world_stream.fail()){ //Probably, no file
        std::cout << "Terrain : Probably, missing terrain file" << file_path << std::endl;
        return false;
    }

    //read dimensions
    world_stream.read(reinterpret_cast<char*>(&this->W), sizeof(int));
    world_stream.read(reinterpret_cast<char*>(&this->H), sizeof(int));

    if(W < 1 || H < 1){
        std::cout << "Terrain : Can't load terrain dimensions from file " << file_path << ", it's probably corrupted!" << std::endl;
        return false;
    }

    //allocate memory
    alloc(W, H);
    //read all terrain points
    for(int i = 0; i < W * H; i ++){
        //Read height
        world_stream.read(reinterpret_cast<char*>(&data[i].height), sizeof(float));
        //Iterate over all textures
        for(int tex_factor = 0; tex_factor < TERRAIN_TEXTURES_AMOUNT; tex_factor ++)
            world_stream.read(reinterpret_cast<char*>(&data[i].texture_factors[tex_factor]), sizeof(unsigned char));
        world_stream.read(reinterpret_cast<char*>(&data[i].grass), sizeof(int));
    }

    world_stream.close();
    return true;
}

void TerrainData::initPhysics(){

    ZSVECTOR3* vertex_pos = new ZSVECTOR3[static_cast<unsigned int>(W * H)];
    int* phys_indices = new int[static_cast<unsigned int>((W - 1) * (H - 1) * 2 * 3)];

    //Iterate over all vertices and fill their vectors
    for(int y = 0; y < H; y ++){
        for(int x = 0; x < W; x ++){
            //Set vertex height
            vertex_pos[x * H + y] = ZSVECTOR3(x, data[x * H + y].height, y);
        }
    }
    //Declare indices iterator
    unsigned int inds = 0;
    //Iterate over all vertices
    for(int y = 0; y < H - 1; y ++){
        for(int x = 0; x < W - 1; x ++){
            phys_indices[inds] = (x * H + y);
            phys_indices[inds + 2] = (x * H + H + y);
            phys_indices[inds + 1] = (x * H + H + y + 1);

            phys_indices[inds + 3] = (x * H + y);
            phys_indices[inds + 5] = (x * H + H + y + 1);
            phys_indices[inds + 4] = (x * H + y + 1);

            inds += 6;
        }
    }

    int numFaces = (W - 1) * (H - 1) * 2;
    int vertStride = sizeof(ZSVECTOR3);
    int indexStride = 3 * sizeof(unsigned int);
    btTriangleIndexVertexArray* va = new btTriangleIndexVertexArray(numFaces, phys_indices, indexStride,  W * H, reinterpret_cast<btScalar*>(vertex_pos), vertStride);
    if(shape != nullptr)
        delete shape;

    this->shape = new btBvhTriangleMeshShape(va, true,  true);

    delete[] vertex_pos;
    delete[] phys_indices;
}


void TerrainData::processTangentSpace(HeightmapVertex* vert_array, unsigned int* indices_array, int indices_num){
    for(int ind_i = 0; ind_i < indices_num ; ind_i += 3){
        HeightmapVertex v1 = vert_array[indices_array[ind_i]];
        HeightmapVertex v2 = vert_array[indices_array[ind_i + 1]];
        HeightmapVertex v3 = vert_array[indices_array[ind_i + 2]];

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
