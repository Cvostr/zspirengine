#include "../../headers/render/zs-mesh.h"

static Engine::Mesh* picked_mesh = nullptr;

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

static Engine::Mesh plane2Dmesh;
static Engine::Mesh uiSprite2Dmesh;
static Engine::Mesh iso_tile2Dmesh;
static Engine::Mesh cube3Dmesh;

Engine::Mesh::Mesh(){

}

void Engine::Mesh::Init() {
    glGenVertexArrays(1, &this->meshVAO);
    glGenBuffers(1, &this->meshVBO);
    glGenBuffers(1, &this->meshEBO);
}

void Engine::Mesh::Destroy() {
    this->alive = false;

    glDeleteVertexArrays(1, &this->meshVAO); //Removevertex arrays
    glDeleteBuffers(1, &this->meshVBO); //Remove vertex buffer
    glDeleteBuffers(1, &this->meshEBO); //Remove index buffer

}

void Engine::setupDefaultMeshes() {
    picked_mesh = nullptr;

    plane2Dmesh.Init(); //Initialize mesh for GL
    plane2Dmesh.setMeshData(plane_verts, plane_inds, 4, 6); //Send plane data

    uiSprite2Dmesh.Init();
    uiSprite2Dmesh.setMeshData(ui_sprite_vertices, plane_inds, 4, 6);

    iso_tile2Dmesh.Init();
    iso_tile2Dmesh.setMeshData(iso_tile_vertices, isotile_ind, 6, 12);

    cube3Dmesh.Init();
    //cube3Dmesh.setMeshData(cube_vertices, 36);
}

void Engine::freeDefaultMeshes(){
    plane2Dmesh.Destroy();
    uiSprite2Dmesh.Destroy();
    iso_tile2Dmesh.Destroy();
    cube3Dmesh.Destroy();
}

Engine::Mesh* Engine::getPlaneMesh2D() {
    return &plane2Dmesh;
}

Engine::Mesh* Engine::getUiSpriteMesh2D() {
    return &uiSprite2Dmesh;
}

Engine::Mesh* Engine::getIsoTileMesh2D(){
    return &iso_tile2Dmesh;
}
Engine::Mesh* Engine::getCubeMesh3D(){
    return &cube3Dmesh;
}

void Engine::Mesh::setMeshData(ZSVERTEX* vertices, unsigned int* indices, unsigned int vertices_num, unsigned int indices_num) {
    this->vertices_num = vertices_num;
    this->indices_num = indices_num;

    glBindVertexArray(this->meshVAO); //Bind vertex array

    glBindBuffer(GL_ARRAY_BUFFER, this->meshVBO); //Bind vertex buffer
    glBufferData(GL_ARRAY_BUFFER, static_cast<int>(vertices_num) * static_cast<int>(sizeof(ZSVERTEX)), vertices, GL_STATIC_DRAW); //send vertices to buffer

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->meshEBO); //Bind index buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<unsigned int>(indices_num) * sizeof(unsigned int), indices, GL_STATIC_DRAW); //Send indices to buffer

    //Vertex pos 3 floats
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), nullptr);
    glEnableVertexAttribArray(0);
    //Vertex UV 2 floats
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
    //Vertex Normals 3 floats
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * 5));
    glEnableVertexAttribArray(2);

    //Vertex Tangents 3 floats
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * 8));
    glEnableVertexAttribArray(3);
    //Vertex Bitangents 3 floats
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * 11));
    glEnableVertexAttribArray(4);

}
void Engine::Mesh::setMeshData(ZSVERTEX* vertices, unsigned int vertices_num) {
    this->vertices_num = vertices_num;
    this->indices_num = NO_INDICES;

    glBindVertexArray(this->meshVAO); //Bind vertex array

    glBindBuffer(GL_ARRAY_BUFFER, this->meshVBO); //Bind vertex buffer
    glBufferData(GL_ARRAY_BUFFER, vertices_num * sizeof(ZSMATRIX4x4), vertices, GL_STATIC_DRAW); //send vertices to buffer

    //Vertex pos 3 floats
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), nullptr);
    glEnableVertexAttribArray(0);
    //Vertex UV 2 floats
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
    //Vertex Normals 3 floats
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * 5));
    glEnableVertexAttribArray(2);

    //Vertex Tangents 3 floats
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * 8));
    glEnableVertexAttribArray(3);
    //Vertex Bitangents 3 floats
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * 11));
    glEnableVertexAttribArray(4);
}

void Engine::Mesh::Draw(){

    if(picked_mesh != this){ //if this mesh wasn't picked
        picked_mesh = this; //Setting pointer of this obj
        glBindVertexArray(this->meshVAO);
        if(this->indices_num != NO_INDICES) //if object uses indices
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->meshEBO);
    }

    if (this->indices_num == NO_INDICES) {
        //Draw without indices
        glDrawArrays(GL_TRIANGLES, 0, this->vertices_num);
    }
    else {
        //Indexed draw
        glDrawElements(GL_TRIANGLES, this->indices_num, GL_UNSIGNED_INT, 0);
    }
}

void Engine::Mesh::DrawLines(){

    if(picked_mesh != this){ //if this mesh wasn't picked
        picked_mesh = this; //Setting pointer of this obj
        glBindVertexArray(this->meshVAO);
        if(this->indices_num != NO_INDICES) //if object uses indices
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->meshEBO);
    }

    if (this->indices_num == NO_INDICES) {
        //Draw without indices
        glDrawArrays(GL_LINE_LOOP, 0, this->vertices_num);
    }
    else {
        //Indexed draw
        glDrawElements(GL_LINE_LOOP, this->indices_num, GL_UNSIGNED_INT, nullptr);
    }
}
