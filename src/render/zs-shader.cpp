#include "../../headers/render/zs-shader.h"
//#include "../World/headers/World.h"
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

Engine::Shader::Shader(){
    isCreated = false; //Not created by default
}

Engine::Shader::~Shader(){

}

bool Engine::Shader::compileFromFile(std::string VSpath, std::string FSpath){
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL32 : {
            static_cast<_ogl_Shader*>(this)->compileFromFile(VSpath, FSpath);
            break;
        }
        case VULKAN : {
            static_cast<_vk_Shader*>(this)->compileFromFile(VSpath, FSpath);
            break;
        }
    }
    this->isCreated = true;
}

void Engine::Shader::Destroy() {
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL32 : {
            static_cast<_ogl_Shader*>(this)->Destroy();
            break;
        }
        case VULKAN : {
            static_cast<_vk_Shader*>(this)->Destroy();
            break;
        }
    }
    this->isCreated = false;
}

void Engine::Shader::Use() {
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL32 : {
            static_cast<_ogl_Shader*>(this)->Use();
            break;
        }
        case VULKAN : {
            static_cast<_vk_Shader*>(this)->Use();
            break;
        }
    }
}

