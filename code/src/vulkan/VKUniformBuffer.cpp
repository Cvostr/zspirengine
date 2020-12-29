#include "../../headers/vulkan/VKUniformBuffer.hpp"
#include "../../headers/game.h"

extern ZSGAME_DATA* game_data;

void Engine::vkUniformBuffer::init(unsigned int slot, unsigned int size, bool CreateCpuBuffer){
    if (!mCreated) {
        mSlot = slot;
        mBufferSize = size;

        mCpuBufferCreated = true;

        game_data->vk_main->mVMA->allocateCpu(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, &mVkBuffer, size, (void**)&this->mCpuBuffer);

        mCreated = true;
    }
}
void Engine::vkUniformBuffer::writeData(unsigned int offset, unsigned int size, void* data){
    writeDataBuffered(offset, size, data);
}

void Engine::vkUniformBuffer::updateBufferedData() {
    
}

void Engine::vkUniformBuffer::bind(){

}

void Engine::vkUniformBuffer::Destroy() {
   // vkDestroyBuffer(game_data->vk_main->mDevice->getVkDevice(), this->mVkBuffer, nullptr);
}

Engine::vkUniformBuffer::vkUniformBuffer(){

}
Engine::vkUniformBuffer::~vkUniformBuffer(){

}
