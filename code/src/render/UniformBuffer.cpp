#include <render/UniformBuffer.hpp>
#include <game.h>

#include <ogl/GLUniformBuffer.hpp>
#include <vulkan/VKUniformBuffer.hpp>

extern ZSpireEngine* engine_ptr;

Engine::UniformBuffer::UniformBuffer():
    mSlot(0),
    mBufferSize(0)
{

}
Engine::UniformBuffer::UniformBuffer(unsigned int slot, unsigned int size, bool StorageBuffer){
    init(slot, size, StorageBuffer);
}

void Engine::UniformBuffer::writeDataBuffered(unsigned int offset, unsigned int size, void* data) {
    memcpy(this->mCpuBuffer + offset, data, size);
}

Engine::UniformBuffer::~UniformBuffer(){

}

Engine::UniformBuffer* Engine::allocUniformBuffer(){
    Engine::UniformBuffer* result = nullptr;
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL: {
            result = new glUniformBuffer;
            break;
        }
        case VULKAN : {
            result = new vkUniformBuffer;
            break;
        }
    }
    return result;
}
