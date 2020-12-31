#include "../../headers/render/Shader.hpp"
#include <GL/glew.h>

#include <cstring>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "../../headers/ogl/GLShader.hpp"
#include "../../headers/vulkan/VKShader.hpp"

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
