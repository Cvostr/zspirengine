#include "../../headers/world/terrain.h"
#include <fstream>
#include <iostream>
#include <GL/glew.h>

TerrainData::TerrainData(){
    GrassDensity = 1.0f;
    W = 0; H = 0;
    created = false;
    hasHeightmapChanged = false;
    hasPaintingChanged = false;
    hasPhysicShapeChanged = false;
    shape = nullptr;
    data = nullptr;

    vertices = nullptr;
    indices = nullptr;
}

bool TerrainData::isCreated() {
    return created;
}

TerrainData::~TerrainData(){
    if(created)
        delete[] data;
    //Free shape
    if(shape)
        delete shape;
    //Free Dynamic Arrays
    delete[] vertices;
    delete[] indices;
    //Free memory for texture buffers
    painting.freeMem();
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

void TerrainData::flatTerrain(float height){
    for(int i = 0; i < W * H; i ++){
        data[i].height = height;
        this->data[i].texture_factors[0] = 255;
        for(int t = 1; t < TERRAIN_TEXTURES_AMOUNT; t++)
            this->data[i].texture_factors[t] = 0;
    }
    //Clear all grass positions
    hasGrassChanged = true;
    updateGrassBuffers();
}

void TerrainData::updateTextureBuffers(bool full_rebuild){
    //write temporary array to store texture ids
    if (full_rebuild) {
        painting._texture = new unsigned char[static_cast<unsigned int>(W * H * 4)];
        painting._texture1 = new unsigned char[static_cast<unsigned int>(W * H * 4)];
        painting._texture2 = new unsigned char[static_cast<unsigned int>(W * H * 4)];
    }
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

void TerrainData::updateGeometryBuffers(bool full_rebuild){
    //Allocate vertices array
    if(vertices == nullptr)
        vertices = new HeightmapVertex[static_cast<unsigned int>(W * H)];
        
    //Iterate over all vertices and fill their vectors
    for(int y = 0; y < H; y ++){
        for(int x = 0; x < W; x ++){
            //Set vertex height
            vertices[x * H + y].pos = ZSVECTOR3(static_cast<float>(x), data[x * H + y].height, static_cast<float>(y));
            //Calculate vertex texture UV
            vertices[x * H + y].uv = ZSVECTOR2(static_cast<float>(x) / W, static_cast<float>(y) / H);
        }
    }
    if (full_rebuild) {
        //Allocate indices array
        if (indices == nullptr)
            indices = new unsigned int[static_cast<unsigned int>((W - 1) * (H - 1) * 2 * 3)];
        //Declare indices iterator
        unsigned int inds = 0;
        //Iterate over all vertices
        for (int y = 0; y < H - 1; y++) {
            for (int x = 0; x < W - 1; x++) {
                indices[inds] = static_cast<unsigned int>(x * H + y);
                indices[inds + 2] = static_cast<unsigned int>(x * H + H + y);
                indices[inds + 1] = static_cast<unsigned int>(x * H + H + y + 1);

                indices[inds + 3] = static_cast<unsigned int>(x * H + y);
                indices[inds + 5] = static_cast<unsigned int>(x * H + H + y + 1);
                indices[inds + 4] = static_cast<unsigned int>(x * H + y + 1);
                //Add inds 6
                inds += 6;
            }
        }
    }
    //generate normals, tangent, bitangent
    processNormalsTangentSpace(vertices, indices, (W - 1) * (H - 1) * 2 * 3);
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
    //Close stream
    world_stream.close();
    return true;
}

bool TerrainData::loadFromMemory(const char* bytes) {
    memcpy(&this->W, bytes, sizeof(int));
    memcpy(&this->H, bytes + 0x4, sizeof(int));

    if (W < 1 || H < 1) {
        return false;
    }
    //allocate memory
    alloc(W, H);
    //read all terrain points
    int offset = 0x8;
    for (int i = 0; i < W * H; i++) {
        //Read height
        memcpy(&data[i].height, bytes + offset, sizeof(float));
        offset += sizeof(float);
        //Iterate over all textures
        for (int tex_factor = 0; tex_factor < TERRAIN_TEXTURES_AMOUNT; tex_factor++) {
            //Read texture factor
            memcpy(&data[i].texture_factors[tex_factor], bytes + offset, sizeof(unsigned char));
            offset += sizeof(unsigned char);
        }
        //Read grass ID
        memcpy(&data[i].grass, bytes + offset, sizeof(int));
        offset += sizeof(int);
    }
    return true;
}

void TerrainData::initPhysics(){
    ZSVECTOR3* vertex_pos = &vertices[0].pos;
    //Calculate amount of indices
    int numFaces = (W - 1) * (H - 1) * 2;
    int vertStride = sizeof(HeightmapVertex);
    int indexStride = 3 * sizeof(unsigned int);
    //Generate VertexArray with geometry
    btTriangleIndexVertexArray* va = new btTriangleIndexVertexArray(numFaces, (int*)indices, indexStride,  W * H, reinterpret_cast<btScalar*>(vertex_pos), vertStride);
    if(shape != nullptr)
        delete shape;
    //Allocate Shape with geometry
    this->shape = new btBvhTriangleMeshShape(va, false,  true);
}

void TerrainData::updateGrassBuffers() {
    //Calculate grass density
    float delta = 1.0f / GrassDensity;
    //Iterate over all grass blocks and clear their transform matrices
    for (unsigned int grass_i = 0; grass_i < grass.size(); grass_i++) {
        grass[grass_i].inst_transform.clear();
    }
    //iterate over all points in terrain
    for (float texelZ = 0; texelZ < W; texelZ += delta) {
        for (float texelX = 0; texelX < H; texelX += delta) {
            //Calculate position offset of grass
            int texelXi = static_cast<int>(floor(texelX));
            int texelZi = static_cast<int>(floor(texelZ));
            //Get vertex, connected to this position
            HeightmapTexel* texel_ptr = &data[texelZi * W + texelXi];
            //if grass exist there
            if (texel_ptr->grass > 0) {
                HeightmapGrass* grass = &this->grass[static_cast<unsigned int>(texel_ptr->grass - 1)];
                ZSVECTOR3 pos = ZSVECTOR3(texelZ, texel_ptr->height, texelX);
                ZSMATRIX4x4 m = getScaleMat(ZSVECTOR3(grass->scale.X, grass->scale.Y, grass->scale.X)) * getTranslationMat(pos);
                grass->inst_transform.push_back(m);
            }
        }
    }
    hasGrassChanged = false;
}

void TerrainData::processNormalsTangentSpace(HeightmapVertex* vert_array, unsigned int* indices_array, int indices_num){
    for(int ind_i = 0; ind_i < indices_num ; ind_i += 3){
        //Get pointer to next terrain vertex
        HeightmapVertex* v1 = &vert_array[indices_array[ind_i]];
        //Normal calculation os so expensive
        if (!data[indices_array[ind_i]].modified && !data[indices_array[ind_i + 1]].modified && !data[indices_array[ind_i + 2]].modified)
            //if vertex was not modified, then avoid normal recalculation
            continue;
        else {
            //Remove "modified" flag
            data[indices_array[ind_i]].modified = false;
        }
        //Get pointers to other vertices of triangle
        HeightmapVertex* v2 = &vert_array[indices_array[ind_i + 1]];
        HeightmapVertex* v3 = &vert_array[indices_array[ind_i + 2]];
        //Poses of other vertices of triangle
        ZSVECTOR3 v12 = v1->pos - v2->pos;
        ZSVECTOR3 v13 = v1->pos - v3->pos;
        //Calculate normal
        v1->normal = vCross(v12, v13);
        //Normalize vector
        vNormalize(&v1->normal);
        //--------------Calculate Tangent 
        ZSVECTOR3 edge1 = v2->pos - v1->pos;
        ZSVECTOR3 edge2 = v3->pos - v1->pos;
        ZSVECTOR2 deltaUV1 = v2->uv - v1->uv;
        ZSVECTOR2 deltaUV2 = v3->uv - v1->uv;

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

void TerrainData::copyTo(TerrainData* dest) {
    //Allocate terrain
    dest->alloc(W, H);
    memcpy(dest->data, data, (static_cast<uint64_t>(W) * H) * sizeof(HeightmapTexel));

    for (unsigned int v = 0; v < static_cast<unsigned int>(W * H); v ++) {
        dest->data[v].modified = true;
    }

    //Generate mesh
    dest->generateGLMesh();

    dest->GrassDensity = GrassDensity;

    for (unsigned int g_i = 0; g_i < grass.size(); g_i++)
        dest->grass.push_back(grass[g_i]);
}