#include "../../headers/render/UniformBuffer.hpp"
#include "../../headers/game.h"

#include "../../headers/ogl/GLUniformBuffer.hpp"
#include "../../headers/vulkan/VKUniformBuffer.hpp"

extern ZSpireEngine* engine_ptr;

Engine::UniformBuffer::UniformBuffer():
    mSlot(0),
    mBufferSize(0),
    mCpuBufferCreated(false)
{

}
Engine::UniformBuffer::UniformBuffer(unsigned int slot, unsigned int size){
    init(slot, size);
}

void Engine::UniformBuffer::writeDataBuffered(unsigned int offset, unsigned int size, void* data) {
    if(mCpuBufferCreated)
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
