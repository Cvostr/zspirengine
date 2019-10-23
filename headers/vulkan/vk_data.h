#ifndef VK_DATA_H
#define VK_DATA_H

#include "../render/zs-mesh.h"
#include "../render/zs-uniform-buffer.h"
#include "../render/zs-shader.h"
#include "../vulkan/zsvulkan.h"

namespace Engine{

class _vk_Mesh : public Engine::Mesh {
private:
    VkBuffer vertexBuffer;
    VkBuffer indexBuffer;
public:
    void Init();
    void setMeshData(ZSVERTEX* vertices, unsigned int* indices, unsigned int vertices_num, unsigned int indices_num);
    void setMeshData(ZSVERTEX* vertices, unsigned int vertices_num);
    void Draw();
    void Destroy();

    _vk_Mesh();
    ~_vk_Mesh();
};

class _vk_UniformBuffer : public UniformBuffer{
private:

public:
    void init(unsigned int slot, unsigned int size);
    void writeData(unsigned int offset, unsigned int size, void* data);
    void bind();

    _vk_UniformBuffer();
    ~_vk_UniformBuffer();
};

class _vk_Shader : public Engine::Shader{
private:
    VkShaderModule vertexShader;
    VkShaderModule fragmentShader;

    VkPipelineShaderStageCreateInfo vertStageInfo;
    VkPipelineShaderStageCreateInfo fragStageInfo;
public:
    bool readBinaryShaderFile(std::string path, char* result, int* size);
    bool compileFromFile(std::string VSpath, std::string FSpath);
    void Use();
    void Destroy();

    _vk_Shader();
    ~_vk_Shader();
};

}


#endif // VK_DATA_H
