#include "../../headers/vulkan/vk_data.h"
#include <fstream>
#include <iostream>

extern ZSpireEngine* engine_ptr;

bool Engine::_vk_Shader::readBinaryShaderFile(std::string path, char** result, size_t& size){
    std::ifstream stream;

    stream.exceptions(std::ifstream::badbit);

    try
    {
        stream.open(path, std::ifstream::binary | std::ios::ate);// Open file
        size_t fileSize = (size_t) stream.tellg(); //tellg returns size
        *result = new char[fileSize];
        stream.seekg(0);
        stream.read(*result, fileSize);

        size = fileSize;

        stream.close();//close file stream
    }
    catch (std::ifstream::failure e)
    {
        return false;
    }
    return true;
}
bool Engine::_vk_Shader::compileFromFile(std::string VSpath, std::string FSpath, std::string GSpath){
    
    size_t vertShaderSize, fragShaderSize, geomShaderSize;

    char* vertShaderData = nullptr;
    char* fragShaderData = nullptr;
    char* geomShaderData = nullptr;

    readBinaryShaderFile(VSpath, &vertShaderData, vertShaderSize);
    readBinaryShaderFile(FSpath, &fragShaderData, fragShaderSize);
    if(GSpath.empty())
        readBinaryShaderFile(GSpath, &geomShaderData, geomShaderSize);

    VkShaderModuleCreateInfo createVsInfo = {};
    createVsInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createVsInfo.pNext = nullptr;
    createVsInfo.codeSize = vertShaderSize;
    createVsInfo.pCode = reinterpret_cast<const uint32_t*>(vertShaderData);
    vkCreateShaderModule(engine_ptr->GetDevice()->getVkDevice(), &createVsInfo, nullptr, &vertexShader);

    VkShaderModuleCreateInfo createFsInfo = {};
    createFsInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createFsInfo.pNext = nullptr;
    createFsInfo.codeSize = fragShaderSize;
    createFsInfo.pCode = reinterpret_cast<const uint32_t*>(fragShaderData);
    vkCreateShaderModule(engine_ptr->GetDevice()->getVkDevice(), &createFsInfo, nullptr, &fragmentShader);

    if (GSpath.empty()) {
        VkShaderModuleCreateInfo createGsInfo = {};
        createGsInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createGsInfo.pNext = nullptr;
        createGsInfo.codeSize = geomShaderSize;
        createGsInfo.pCode = reinterpret_cast<const uint32_t*>(geomShaderData);
        vkCreateShaderModule(engine_ptr->GetDevice()->getVkDevice(), &createGsInfo, nullptr, &geometryShader);
    }

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
