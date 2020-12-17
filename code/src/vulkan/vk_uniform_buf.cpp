#include "../../headers/vulkan/vk_data.h"

void Engine::_vk_UniformBuffer::init(unsigned int slot, unsigned int size){
    if (!mCreated) {
        mSlot = slot;
        mBufferSize = size;

        mCreated = true;
    }
}
void Engine::_vk_UniformBuffer::writeData(unsigned int offset, unsigned int size, void* data){

}
void Engine::_vk_UniformBuffer::bind(){

}

Engine::_vk_UniformBuffer::_vk_UniformBuffer(){

}
Engine::_vk_UniformBuffer::~_vk_UniformBuffer(){

}
