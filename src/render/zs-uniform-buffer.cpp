#include "../../headers/render/zs-uniform-buffer.h"
#include "../../headers/game.h"

#include "../../headers/ogl/ogl.h"
#include "../../headers/vulkan/vk_data.h"

extern ZSpireEngine* engine_ptr;

void Engine::UniformBuffer::init(int slot, int size){
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

Engine::UniformBuffer::UniformBuffer(){

}
Engine::UniformBuffer::UniformBuffer(int slot, int size){
    init(slot, size);
}
Engine::UniformBuffer::~UniformBuffer(){

}
