#include "../../headers/ogl/GLMesh.hpp"


Engine::glMesh::glMesh(){

}

Engine::glMesh::~glMesh(){

}

void Engine::glMesh::Init() {
    if (!mCreated) {
        glGenVertexArrays(1, &this->meshVAO);
        glGenBuffers(1, &this->meshVBO);
        glGenBuffers(1, &this->meshEBO);
        mCreated = true;
    }
}

void Engine::glMesh::Destroy() {
    if (mCreated) {
        glDeleteVertexArrays(1, &this->meshVAO); //Removevertex arrays
        glDeleteBuffers(1, &this->meshVBO); //Remove vertex buffer
        glDeleteBuffers(1, &this->meshEBO); //Remove index buffer
        mCreated = false;
    }
}

void Engine::glMesh::setMeshData(ZSVERTEX* vertices, unsigned int* indices, unsigned int vertices_num, unsigned int indices_num){
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
void Engine::glMesh::setMeshData(ZSVERTEX* vertices, unsigned int vertices_num){
    this->mVerticesNum = static_cast<int>(vertices_num);
    this->mIndicesNum = NO_INDICES;

    glBindVertexArray(this->meshVAO); //Bind vertex array

    glBindBuffer(GL_ARRAY_BUFFER, this->meshVBO); //Bind vertex buffer
    glBufferData(GL_ARRAY_BUFFER, vertices_num * sizeof(ZSVERTEX), vertices, GL_STATIC_DRAW); //send vertices to buffer
    //Bind mesh offsets
    setMeshOffsets();

    mBoundingBox.CreateFromVertexArray(vertices, mVerticesNum);
}

void Engine::glMesh::setMeshData(void* vertices, int vertSize, unsigned int vertices_num) {

}
void Engine::glMesh::setMeshData(void* vertices, int vertSize, unsigned int* indices, unsigned int vertices_num, unsigned int indices_num) {
    this->mVerticesNum = static_cast<int>(vertices_num);
    this->mIndicesNum = static_cast<int>(indices_num);

    glBindVertexArray(this->meshVAO); //Bind vertex array

    glBindBuffer(GL_ARRAY_BUFFER, this->meshVBO); //Bind vertex buffer
    glBufferData(GL_ARRAY_BUFFER, vertSize * vertices_num, vertices, GL_STATIC_DRAW); //send vertices to buffer

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->meshEBO); //Bind index buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<unsigned int>(indices_num) * sizeof(unsigned int), indices, GL_STATIC_DRAW); //Send indices to buffer

    //mBoundingBox.CreateFromVertexArray(vertices, mVerticesNum);
}

void Engine::glMesh::_glVertexAttribPointer(int index, int elems_count, int format, int normalized, int structSize, void* offset) {
    glVertexAttribPointer(index, elems_count, format, normalized, structSize, offset);
    glEnableVertexAttribArray(index);
}

void Engine::glMesh::_glVertexAttribIPointer(int index, int elems_count, int format, int structSize, void* offset) {
    glVertexAttribIPointer(index, elems_count, format, structSize, offset);
    glEnableVertexAttribArray(index);
}

void Engine::glMesh::setMeshOffsets(){
    //Vertex pos 3 floats
    _glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), nullptr);
    //Vertex UV 2 floats
    _glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * 3));
    //Vertex Normals 3 floats
    _glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * 5));
    //Vertex Tangents 3 floats
    _glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * 8));
    //Vertex Bitangents 3 floats
    _glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * 11));

    for(unsigned int i = 0; i < MAX_BONE_PER_VERTEX / 4; i ++){
        //Skinning data
        _glVertexAttribIPointer(5 + i, 4, GL_UNSIGNED_INT, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * (14 + i * 4)));
    }

    for(unsigned int i = 0; i < MAX_BONE_PER_VERTEX / 4; i ++){
        //Weights
        _glVertexAttribPointer(8 + i, 4, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * (26 + i * 4)));
    }

    //Bones num
    _glVertexAttribIPointer(11, 1, GL_UNSIGNED_INT, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * 38));
}

void Engine::glMesh::Draw(){
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

void Engine::glMesh::DrawInstanced(unsigned int instances) {
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

void Engine::glMesh::DrawLines(){

    glBindVertexArray(this->meshVAO);
    if(this->mIndicesNum != NO_INDICES) //if object uses indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->meshEBO);

    if (this->mIndicesNum == NO_INDICES) {
        //Draw without indices
        glDrawArrays(GL_LINE_LOOP, 0, this->mVerticesNum);
    }
    else {
        //Indexed draw
        glDrawElements(GL_LINE_LOOP, this->mVerticesNum, GL_UNSIGNED_INT, nullptr);
    }
}
