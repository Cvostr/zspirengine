#include "../../headers/render/UniformBuffer.hpp"
#include "../../headers/game.h"

#include "../../headers/ogl/ogl.h"
#include "../../headers/vulkan/vk_data.h"

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
            result = new _ogl_UniformBuffer;
            break;
        }
        case VULKAN : {
            result = new _vk_UniformBuffer;
            break;
        }
    }
    return result;
}
