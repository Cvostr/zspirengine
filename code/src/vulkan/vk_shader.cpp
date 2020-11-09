#include "../../headers/vulkan/vk_data.h"
#include <fstream>
#include <iostream>

extern ZSpireEngine* engine_ptr;

bool Engine::_vk_Shader::readBinaryShaderFile(std::string path, char* result, size_t* size){
    std::ifstream stream;

        stream.exceptions(std::ifstream::badbit);

        try
        {
            stream.open(path, std::ifstream::binary | std::ios::ate);// Open file
            size_t fileSize = (size_t) stream.tellg(); //tellg returns size
            stream.seekg(0);
            stream.read(result, fileSize);

            *size = fileSize;

            stream.close();//close file stream
        }
        catch (std::ifstream::failure e)
        {
            return false;
        }
        return true;
}
bool Engine::_vk_Shader::compileFromFile(std::string VSpath, std::string FSpath){
    
    return true;
}
void Engine::_vk_Shader::Use(){

}
void Engine::_vk_Shader::Destroy(){

}

Engine::_vk_Shader::_vk_Shader(){

}
Engine::_vk_Shader::~_vk_Shader(){

}
