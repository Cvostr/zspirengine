#include <vulkan/VKMesh.hpp>
#include <game.h>

extern ZSGAME_DATA* game_data;

Engine::vkMesh::vkMesh(){

}

Engine::vkMesh::~vkMesh(){
    Destroy();
}

void Engine::vkMesh::Init() {

}

void Engine::vkMesh::Destroy() {
    if (mCreated) {
        game_data->vk_main->mVMA->destroyBuffer(&vertexBuffer);
        if(this->mIndicesNum == NO_INDICES)
            game_data->vk_main->mVMA->destroyBuffer(&indexBuffer);
        mCreated = false;
    }
}

void Engine::vkMesh::setMeshData(ZSVERTEX* vertices, unsigned int* indices, unsigned int vertices_num, unsigned int indices_num){
    mVerticesNum = vertices_num;
    mIndicesNum = indices_num;

    //Allocate buffer
    game_data->vk_main->mVMA->allocate(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, &this->vertexBuffer, vertices, vertices_num * sizeof(ZSVERTEX));
    game_data->vk_main->mVMA->allocate(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, &this->indexBuffer, indices, indices_num * sizeof(unsigned int));

    mBoundingBox.CreateFromVertexArray(vertices, mVerticesNum);

    mCreated = true;
}
void Engine::vkMesh::setMeshData(ZSVERTEX* vertices, unsigned int vertices_num){

    mVerticesNum = vertices_num;
    //Allocate buffer
    void* vert;
    game_data->vk_main->mVMA->allocateCpu(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, &this->vertexBuffer, vertices_num * sizeof(ZSVERTEX), &vert);
    memcpy(vert, vertices, vertices_num * sizeof(ZSVERTEX));
    game_data->vk_main->mVMA->unmap(&vertexBuffer);

    mBoundingBox.CreateFromVertexArray(vertices, mVerticesNum);

    mCreated = true;
}

void Engine::vkMesh::Draw() {
    Draw(game_data->vk_main->CurrentCmdBuffer);
}

void Engine::vkMesh::DrawInstanced(unsigned int instances) {
    DrawInstanced(game_data->vk_main->CurrentCmdBuffer, instances);
}

void Engine::vkMesh::DrawInstanced(VkCommandBuffer CmdBuf, unsigned int instances) {
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(CmdBuf, 0, 1, &this->vertexBuffer.Buffer, offsets);

    if (this->mIndicesNum != NO_INDICES) { //if object uses indices
        vkCmdBindIndexBuffer(CmdBuf, indexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);
        //Indexed draw
        vkCmdDrawIndexed(CmdBuf, mIndicesNum, instances, 0, 0, 0);
    }
    else {
        //Draw without indices
        vkCmdDraw(CmdBuf, mVerticesNum, instances, 0, 0);
    }
}

void Engine::vkMesh::Draw(VkCommandBuffer CmdBuf){
    DrawInstanced(game_data->vk_main->CurrentCmdBuffer, 1);
}
