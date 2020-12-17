#include "../../headers/vulkan/vk_data.h"
#include "../../headers/game.h"

extern ZSGAME_DATA* game_data;

Engine::_vk_Mesh::_vk_Mesh(){

}

Engine::_vk_Mesh::~_vk_Mesh(){

}

void Engine::_vk_Mesh::Init() {

}

void Engine::_vk_Mesh::Destroy() {

}

void Engine::_vk_Mesh::setMeshData(ZSVERTEX* vertices, unsigned int* indices, unsigned int vertices_num, unsigned int indices_num){
    mVerticesNum = vertices_num;
    mIndicesNum = indices_num;

    //Allocate buffer
    game_data->vk_main->mVMA->allocate(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, &this->vertexBuffer, vertices, vertices_num * sizeof(ZSVERTEX));
    game_data->vk_main->mVMA->allocate(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, &this->indexBuffer, indices, indices_num * sizeof(unsigned int));
}
void Engine::_vk_Mesh::setMeshData(ZSVERTEX* vertices, unsigned int vertices_num){

    mVerticesNum = vertices_num;
    //Allocate buffer
    game_data->vk_main->mVMA->allocate(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, &this->vertexBuffer, vertices, vertices_num * sizeof(ZSVERTEX));
}
void Engine::_vk_Mesh::Draw(VkCommandBuffer CmdBuf){
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(CmdBuf, 0, 1, &this->vertexBuffer, offsets);

    if (this->mIndicesNum != NO_INDICES) //if object uses indices
        vkCmdBindIndexBuffer(CmdBuf, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    if (this->mIndicesNum == NO_INDICES) {
        //Draw without indices
        vkCmdDraw(CmdBuf, mVerticesNum, 1, 0, 0);
    }
    else {
        //Indexed draw
        vkCmdDrawIndexed(CmdBuf, mIndicesNum, 1, 0, 0, 0);
    }
}
