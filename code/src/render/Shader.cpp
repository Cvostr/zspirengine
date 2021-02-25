#include <render/Shader.hpp>
#include <GL/glew.h>

#include <ogl/GLShader.hpp>
#include <vulkan/VKShader.hpp>

extern ZSpireEngine* engine_ptr;

Engine::Shader* Engine::allocShader(){
    Engine::Shader* result = nullptr;
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL: {
            result = new glShader;
            break;
        }
        case VULKAN : {
            result = new vkShader;
            break;
        }
    }
    return result;
}
