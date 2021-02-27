#include <vulkan/VKShader.hpp>
#include <game.h>
#include <fstream>
#include <iostream>

extern ZSGAME_DATA* game_data;

bool Engine::vkShader::readBinaryShaderFile(std::string path, char** result, size_t& size){
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
bool Engine::vkShader::compileFromFile(std::string VSpath,
                                        std::string FSpath,
                                        std::string GSpath,
                                        std::string TCSpath,
                                        std::string TESpath
){
    
    size_t vertShaderSize, fragShaderSize, geomShaderSize, tcsShaderSize, tesShaderSize;

    char* vertShaderData = nullptr;
    char* fragShaderData = nullptr;
    char* geomShaderData = nullptr;
    char* tcsShaderData = nullptr;
    char* tesShaderData = nullptr;

    mStages = HAS_VERT_SHADER;

    readBinaryShaderFile(VSpath, &vertShaderData, vertShaderSize);
    if (!FSpath.empty()) {
        mStages |= HAS_FRAG_SHADER;
        readBinaryShaderFile(FSpath, &fragShaderData, fragShaderSize);
    }
    if (!GSpath.empty()) {
        readBinaryShaderFile(GSpath, &geomShaderData, geomShaderSize);
        mStages |= HAS_GEOM_SHADER;
    }
    if (!TCSpath.empty()) {
        readBinaryShaderFile(TCSpath, &tcsShaderData, tcsShaderSize);
        mStages |= HAS_TESSCTRL_SHADER;
    }
    if (!TESpath.empty()) {
        readBinaryShaderFile(TESpath, &tesShaderData, tesShaderSize);
        mStages |= HAS_TESS_SHADER;
    }

    VkShaderModuleCreateInfo createVsInfo = {};
    createVsInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createVsInfo.pNext = nullptr;
    createVsInfo.codeSize = vertShaderSize;
    createVsInfo.pCode = reinterpret_cast<const uint32_t*>(vertShaderData);
    vkCreateShaderModule(game_data->vk_main->mDevice->getVkDevice(), &createVsInfo, nullptr, &vertexShader);

    if (mStages & HAS_FRAG_SHADER) {
        VkShaderModuleCreateInfo createFsInfo = {};
        createFsInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createFsInfo.pNext = nullptr;
        createFsInfo.codeSize = fragShaderSize;
        createFsInfo.pCode = reinterpret_cast<const uint32_t*>(fragShaderData);
        vkCreateShaderModule(game_data->vk_main->mDevice->getVkDevice(), &createFsInfo, nullptr, &fragmentShader);
    }
    if (mStages & HAS_GEOM_SHADER) {
        VkShaderModuleCreateInfo createGsInfo = {};
        createGsInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createGsInfo.pNext = nullptr;
        createGsInfo.codeSize = geomShaderSize;
        createGsInfo.pCode = reinterpret_cast<const uint32_t*>(geomShaderData);
        vkCreateShaderModule(game_data->vk_main->mDevice->getVkDevice(), &createGsInfo, nullptr, &geometryShader);
    }
    if (mStages & HAS_TESSCTRL_SHADER) {
        VkShaderModuleCreateInfo createTCSInfo = {};
        createTCSInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createTCSInfo.pNext = nullptr;
        createTCSInfo.codeSize = tcsShaderSize;
        createTCSInfo.pCode = reinterpret_cast<const uint32_t*>(tcsShaderData);
        vkCreateShaderModule(game_data->vk_main->mDevice->getVkDevice(), &createTCSInfo, nullptr, &tessControlShader);
    }
    if (mStages & HAS_TESS_SHADER) {
        VkShaderModuleCreateInfo createTESInfo = {};
        createTESInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createTESInfo.pNext = nullptr;
        createTESInfo.codeSize = tesShaderSize;
        createTESInfo.pCode = reinterpret_cast<const uint32_t*>(tesShaderData);
        vkCreateShaderModule(game_data->vk_main->mDevice->getVkDevice(), &createTESInfo, nullptr, &tessEvalShader);
    }

    mCreated = true;

    return true;
}
void Engine::vkShader::Use(){

}
void Engine::vkShader::Destroy(){

}

Engine::vkShader::vkShader(){

}
Engine::vkShader::~vkShader(){

}
