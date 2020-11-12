#include "../../headers/render/Shader.hpp"
#include "../../headers/world/go_properties.h"
#include <GL/glew.h>

#include <cstring>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "../../headers/ogl/ogl.h"
#include "../../headers/vulkan/vk_data.h"

extern ZSpireEngine* engine_ptr;

Engine::Shader* Engine::allocShader(){
    Engine::Shader* result = nullptr;
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL32 : {
            result = new _ogl_Shader;
            break;
        }
        case VULKAN : {
            result = new _vk_Shader;
            break;
        }
    }
    return result;
}
