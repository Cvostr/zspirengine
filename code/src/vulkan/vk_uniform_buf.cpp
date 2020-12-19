#include "../../headers/vulkan/vk_data.h"
#include "../../headers/game.h"

extern ZSGAME_DATA* game_data;

void Engine::_vk_UniformBuffer::init(unsigned int slot, unsigned int size, bool CreateCpuBuffer){
    if (!mCreated) {
        mSlot = slot;
        mBufferSize = size;

        if (CreateCpuBuffer) {
            mCpuBuffer = new char[mBufferSize];
            mCpuBufferCreated = true;
        }

        game_data->vk_main->mVMA->allocate(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, &this->mVkBuffer, size);

        mCreated = true;
    }
}
void Engine::_vk_UniformBuffer::writeData(unsigned int offset, unsigned int size, void* data){

}

void Engine::_vk_UniformBuffer::updateBufferedData() {

}

void Engine::_vk_UniformBuffer::bind(){

}

Engine::_vk_UniformBuffer::_vk_UniformBuffer(){

}
Engine::_vk_UniformBuffer::~_vk_UniformBuffer(){

}
