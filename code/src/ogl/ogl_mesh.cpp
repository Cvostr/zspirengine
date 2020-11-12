#include "../../headers/ogl/ogl.h"


Engine::_ogl_Mesh::_ogl_Mesh(){

}

Engine::_ogl_Mesh::~_ogl_Mesh(){

}

void Engine::_ogl_Mesh::Init() {
    glGenVertexArrays(1, &this->meshVAO);
    glGenBuffers(1, &this->meshVBO);
    glGenBuffers(1, &this->meshEBO);
}

void Engine::_ogl_Mesh::Destroy() {
    this->alive = false;

    glDeleteVertexArrays(1, &this->meshVAO); //Removevertex arrays
    glDeleteBuffers(1, &this->meshVBO); //Remove vertex buffer
    glDeleteBuffers(1, &this->meshEBO); //Remove index buffer

}

void Engine::_ogl_Mesh::setMeshData(ZSVERTEX* vertices, unsigned int* indices, unsigned int vertices_num, unsigned int indices_num){
    this->mVerticesNum = static_cast<int>(vertices_num);
    this->mIndicesNum = static_cast<int>(indices_num);

    glBindVertexArray(this->meshVAO); //Bind vertex array

    glBindBuffer(GL_ARRAY_BUFFER, this->meshVBO); //Bind vertex buffer
    glBufferData(GL_ARRAY_BUFFER, static_cast<int>(vertices_num) * static_cast<int>(sizeof(ZSVERTEX)), vertices, GL_STATIC_DRAW); //send vertices to buffer

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->meshEBO); //Bind index buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<unsigned int>(indices_num) * sizeof(unsigned int), indices, GL_STATIC_DRAW); //Send indices to buffer

    setMeshOffsets();

    mBoundingBox.CreateFromVertexArray(vertices, mVerticesNum);
}
void Engine::_ogl_Mesh::setMeshData(ZSVERTEX* vertices, unsigned int vertices_num){
    this->mVerticesNum = static_cast<int>(vertices_num);
    this->mIndicesNum = NO_INDICES;

    glBindVertexArray(this->meshVAO); //Bind vertex array

    glBindBuffer(GL_ARRAY_BUFFER, this->meshVBO); //Bind vertex buffer
    glBufferData(GL_ARRAY_BUFFER, vertices_num * sizeof(ZSVERTEX), vertices, GL_STATIC_DRAW); //send vertices to buffer
    //Bind mesh offsets
    setMeshOffsets();

    mBoundingBox.CreateFromVertexArray(vertices, mVerticesNum);
}

void Engine::_ogl_Mesh::setMeshOffsets(){
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

    for(unsigned int i = 0; i < MAX_BONE_PER_VERTEX / 4; i ++){
        //Skinning data
        glVertexAttribIPointer(5 + i, 4, GL_UNSIGNED_INT, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * (14 + i * 4)));
        glEnableVertexAttribArray(5 + i);
    }

    for(unsigned int i = 0; i < MAX_BONE_PER_VERTEX / 4; i ++){
        //Weights
        glVertexAttribPointer(8 + i, 4, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * (26 + i * 4)));
        glEnableVertexAttribArray(8 + i);
    }

    //Bones num
    glVertexAttribIPointer(11, 1, GL_UNSIGNED_INT, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * 38));
    glEnableVertexAttribArray(11);
}

void Engine::_ogl_Mesh::Draw(){
    glBindVertexArray(this->meshVAO);
    if(this->mIndicesNum != NO_INDICES) //if object uses indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->meshEBO);

    if (this->mIndicesNum == NO_INDICES) {
        //Draw without indices
        glDrawArrays(GL_TRIANGLES, 0, this->mVerticesNum);
    }
    else {
        //Indexed draw
        glDrawElements(GL_TRIANGLES, this->mIndicesNum, GL_UNSIGNED_INT, nullptr);
    }
}

void Engine::_ogl_Mesh::DrawInstanced(unsigned int instances) {
    glBindVertexArray(this->meshVAO);
    if (this->mIndicesNum != NO_INDICES) //if object uses indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->meshEBO);

    if (this->mIndicesNum == NO_INDICES) {
        //Draw without indices
        glDrawArraysInstanced(GL_TRIANGLES, 0, this->mVerticesNum, instances);
    }
    else {
        //Indexed draw
        glDrawElementsInstanced(GL_TRIANGLES, this->mIndicesNum, GL_UNSIGNED_INT, nullptr, instances);
    }
}

void Engine::_ogl_Mesh::DrawLines(){

    glBindVertexArray(this->meshVAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->meshVBO);
    if(this->mIndicesNum != NO_INDICES) //if object uses indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->meshEBO);

    if (this->mIndicesNum == NO_INDICES) {
        //Draw without indices
        glDrawArrays(GL_LINE_LOOP, 0, static_cast<int>(this->mVerticesNum));
    }
    else {
        //Indexed draw
        glDrawElements(GL_LINE_LOOP, static_cast<int>(this->mVerticesNum), GL_UNSIGNED_INT, nullptr);
    }
}
