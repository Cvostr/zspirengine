#include "../../headers/vulkan/VKMesh.hpp"
#include "../../headers/game.h"

extern ZSGAME_DATA* game_data;

Engine::vkMesh::vkMesh(){

}

Engine::vkMesh::~vkMesh(){

}

void Engine::vkMesh::Init() {

}

void Engine::vkMesh::Destroy() {
    game_data->vk_main->mVMA->destroyBuffer(&vertexBuffer);
}

void Engine::vkMesh::setMeshData(ZSVERTEX* vertices, unsigned int* indices, unsigned int vertices_num, unsigned int indices_num){
    mVerticesNum = vertices_num;
    mIndicesNum = indices_num;

    //Allocate buffer
    game_data->vk_main->mVMA->allocate(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, &this->vertexBuffer, vertices, vertices_num * sizeof(ZSVERTEX));
    game_data->vk_main->mVMA->allocate(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, &this->indexBuffer, indices, indices_num * sizeof(unsigned int));
    /*void* vert;
    void* ind;
    game_data->vk_main->mVMA->allocateCpu(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, &this->vertexBuffer, vertices_num * sizeof(ZSVERTEX), (void**)&vert);
    game_data->vk_main->mVMA->allocateCpu(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, &this->indexBuffer, indices_num * sizeof(unsigned int), (void**)&ind);

    memcpy(vert, vertices, vertices_num * sizeof(ZSVERTEX));
    memcpy(ind, indices, indices_num * sizeof(unsigned int));

    game_data->vk_main->mVMA->unmap(&vertexBuffer);
    game_data->vk_main->mVMA->unmap(&indexBuffer);*/
}
void Engine::vkMesh::setMeshData(ZSVERTEX* vertices, unsigned int vertices_num){

    mVerticesNum = vertices_num;
    //Allocate buffer
    void* vert;
    game_data->vk_main->mVMA->allocateCpu(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, &this->vertexBuffer, vertices_num * sizeof(ZSVERTEX), &vert);
    memcpy(vert, vertices, vertices_num * sizeof(ZSVERTEX));
    game_data->vk_main->mVMA->unmap(&vertexBuffer);
}

void Engine::vkMesh::Draw() {
    Draw(game_data->vk_main->CurrentCmdBuffer);
}

void Engine::vkMesh::DrawInstanced(unsigned int instances) {
    Draw(game_data->vk_main->CurrentCmdBuffer);
}

void Engine::vkMesh::Draw(VkCommandBuffer CmdBuf){
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(CmdBuf, 0, 1, &this->vertexBuffer.Buffer, offsets);

    if (this->mIndicesNum != NO_INDICES){ //if object uses indices
        vkCmdBindIndexBuffer(CmdBuf, indexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);
        //Indexed draw
        vkCmdDrawIndexed(CmdBuf, mIndicesNum, 1, 0, 0, 0);
    }else {
        //Draw without indices
        vkCmdDraw(CmdBuf, mVerticesNum, 1, 0, 0);
    }
}
