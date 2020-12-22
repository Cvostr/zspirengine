#include "../../headers/vulkan/vk_data.h"
#include "../../headers/game.h"

extern ZSGAME_DATA* game_data;

void Engine::_vk_UniformBuffer::init(unsigned int slot, unsigned int size, bool CreateCpuBuffer){
    if (!mCreated) {
        mSlot = slot;
        mBufferSize = size;

        mCpuBufferCreated = true;

        game_data->vk_main->mVMA->allocateCpu(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, &mVkBuffer, size, (void**)&this->mCpuBuffer);

        mCreated = true;
    }
}
void Engine::_vk_UniformBuffer::writeData(unsigned int offset, unsigned int size, void* data){
    writeDataBuffered(offset, size, data);
}

void Engine::_vk_UniformBuffer::updateBufferedData() {
    
}

void Engine::_vk_UniformBuffer::bind(){

}

void Engine::_vk_UniformBuffer::Destroy() {
   // vkDestroyBuffer(game_data->vk_main->mDevice->getVkDevice(), this->mVkBuffer, nullptr);
}

Engine::_vk_UniformBuffer::_vk_UniformBuffer(){

}
Engine::_vk_UniformBuffer::~_vk_UniformBuffer(){

}
