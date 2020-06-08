#include "../../headers/render/zs-uniform-buffer.h"
#include "../../headers/game.h"

#include "../../headers/ogl/ogl.h"
#include "../../headers/vulkan/vk_data.h"

extern ZSpireEngine* engine_ptr;

void Engine::UniformBuffer::init(unsigned int slot, unsigned int size){
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL32 : {
            static_cast<_ogl_UniformBuffer*>(this)->init(slot, size);
            break;
        }
        case VULKAN : {
            static_cast<_vk_UniformBuffer*>(this)->init(slot, size);
            break;
        }
    }
}
void Engine::UniformBuffer::writeData(unsigned int offset, unsigned int size, void* data){
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL32 : {
            static_cast<_ogl_UniformBuffer*>(this)->writeData(offset, size, data);
            break;
        }
        case VULKAN : {
            static_cast<_vk_UniformBuffer*>(this)->writeData(offset, size, data);
            break;
        }
    }
}

void Engine::UniformBuffer::bind(){
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL32 : {
            static_cast<_ogl_UniformBuffer*>(this)->bind();
            break;
        }
        case VULKAN : {
            static_cast<_vk_UniformBuffer*>(this)->bind();
            break;
        }
    }
}

void Engine::UniformBuffer::Destroy(){
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL32 : {
            static_cast<_ogl_UniformBuffer*>(this)->Destroy();
            break;
        }
        case VULKAN : {
            static_cast<_vk_UniformBuffer*>(this)->Destroy();
            break;
        }
    }
}

Engine::UniformBuffer::UniformBuffer(){

}
Engine::UniformBuffer::UniformBuffer(unsigned int slot, unsigned int size){
    init(slot, size);
}
Engine::UniformBuffer::~UniformBuffer(){

}

Engine::UniformBuffer* Engine::allocUniformBuffer(){
    Engine::UniformBuffer* result = nullptr;
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL32 : {
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
