#include "../../headers/vulkan/vk_data.h"
#include <fstream>
#include <iostream>

extern ZSpireEngine* engine_ptr;

bool Engine::_vk_Shader::readBinaryShaderFile(std::string path, char* result, int* size){
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
    std::cout << "VULKAN: Compiling shader " << VSpath << "_vk" << " " << FSpath << "_vk" << std::endl;

    ZsVulkan* vk = engine_ptr->getVulkanContext();

    char* VScontent = new char[SHADER_STR_LEN];
    char* FScontent = new char[SHADER_STR_LEN];

    int VS_size, FS_size = 0;
    //Trying to read binary shader data
    if(!readBinaryShaderFile(VSpath + "_vk", VScontent, &VS_size) || !readBinaryShaderFile(FSpath + "_vk", FScontent, &FS_size))
        return false;

    if(VS_size < 0 || FS_size < 0)
        return false;

    VkShaderModuleCreateInfo createVsInfo = {};
    createVsInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createVsInfo.pNext = nullptr;
    createVsInfo.codeSize = VS_size;
    createVsInfo.pCode = reinterpret_cast<const uint32_t*>(VScontent);
    vkCreateShaderModule(vk->getVkDevice(), &createVsInfo, nullptr, &vertexShader);

    VkShaderModuleCreateInfo createFsInfo = {};
    createFsInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createFsInfo.pNext = nullptr;
    createFsInfo.codeSize = FS_size;
    createFsInfo.pCode = reinterpret_cast<const uint32_t*>(FScontent);
    vkCreateShaderModule(vk->getVkDevice(), &createFsInfo, nullptr, &fragmentShader);

    this->pipeline = new ZsVkPipeline;

    ZsVkPipelineConf conf;

    if(!pipeline->create(this, conf, engine_ptr->getVulkanContext()))
        return false;

    std::cout << "Vulkan : Sheder compiling successful!" << std::endl;

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
