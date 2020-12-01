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
    VkBufferCreateInfo vertexBufferCreateInf;
    vertexBufferCreateInf.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vertexBufferCreateInf.pNext = nullptr;
    vertexBufferCreateInf.flags = 0;
    vertexBufferCreateInf.size = vertices_num * sizeof (ZSVERTEX);
    vertexBufferCreateInf.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    vertexBufferCreateInf.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkBufferCreateInfo indexBufferCreateInf;
    indexBufferCreateInf.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    indexBufferCreateInf.pNext = nullptr;
    indexBufferCreateInf.flags = 0;
    indexBufferCreateInf.size = indices_num * sizeof (unsigned int);
    indexBufferCreateInf.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    indexBufferCreateInf.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    //Allocate buffer
    game_data->vk_main->mVMA->allocate(vertexBufferCreateInf, &this->vertexBuffer);
    game_data->vk_main->mVMA->allocate(indexBufferCreateInf, &this->indexBuffer);
}
void Engine::_vk_Mesh::setMeshData(ZSVERTEX* vertices, unsigned int vertices_num){
    VkBufferCreateInfo vertexBufferCreateInf;
    vertexBufferCreateInf.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vertexBufferCreateInf.pNext = nullptr;
    vertexBufferCreateInf.flags = 0;
    vertexBufferCreateInf.size = vertices_num * sizeof (ZSVERTEX);
    vertexBufferCreateInf.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    vertexBufferCreateInf.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    //Allocate buffer
    game_data->vk_main->mVMA->allocate(vertexBufferCreateInf, &this->vertexBuffer);
}
void Engine::_vk_Mesh::Draw(){

}
