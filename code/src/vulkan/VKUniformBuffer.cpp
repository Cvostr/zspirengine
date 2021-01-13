#include "../../headers/vulkan/VKUniformBuffer.hpp"
#include "../../headers/game.h"

extern ZSGAME_DATA* game_data;

void Engine::vkUniformBuffer::init(unsigned int slot, unsigned int size, bool StorageBuffer){
    if (!mCreated) {
        mSlot = slot;
        mBufferSize = size;

        Usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        if (StorageBuffer)
            Usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

        game_data->vk_main->mVMA->allocateCpu(Usage, &mVkBuffer, size, (void**)&this->mCpuBuffer);

        mCreated = true;
    }
}
void Engine::vkUniformBuffer::writeData(unsigned int offset, unsigned int size, void* data){
    if(mCreated)
        writeDataBuffered(offset, size, data);
}

void Engine::vkUniformBuffer::updateBufferedData() {
    
}

void Engine::vkUniformBuffer::bind(){

}

void Engine::vkUniformBuffer::Destroy() {
    if (!mCreated) {
        game_data->vk_main->mVMA->destroyBuffer(&mVkBuffer);
        mCreated = false;
    }
}

Engine::vkUniformBuffer::vkUniformBuffer(){

}
Engine::vkUniformBuffer::~vkUniformBuffer(){

}
