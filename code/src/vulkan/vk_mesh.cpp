#include "../../headers/vulkan/vk_data.h"
#include "../../headers/vulkan/zsvk_mem.h"

extern ZsVulkan* vulkan_ptr;

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

    VkResult vertex_result = vkCreateBuffer(vulkan_ptr->getVkDevice(), &vertexBufferCreateInf, nullptr, &this->vertexBuffer);
    VkResult index_result = vkCreateBuffer(vulkan_ptr->getVkDevice(), &indexBufferCreateInf, nullptr, &this->indexBuffer);

    VkMemoryRequirements req;
    vkGetBufferMemoryRequirements(vulkan_ptr->getVkDevice(), vertexBuffer, &req);

    //VkPhysicalDeviceMemoryProperties props = getPhysDeviceMemoryProps();
}
void Engine::_vk_Mesh::setMeshData(ZSVERTEX* vertices, unsigned int vertices_num){
    VkBufferCreateInfo vertexBufferCreateInf;
    vertexBufferCreateInf.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vertexBufferCreateInf.pNext = nullptr;
    vertexBufferCreateInf.flags = 0;
    vertexBufferCreateInf.size = vertices_num * sizeof (ZSVERTEX);
    vertexBufferCreateInf.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    vertexBufferCreateInf.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult vertex_result = vkCreateBuffer(vulkan_ptr->getVkDevice(), &vertexBufferCreateInf, nullptr, &this->vertexBuffer);

}
void Engine::_vk_Mesh::Draw(){

}
