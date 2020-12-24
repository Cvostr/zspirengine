#ifndef VK_DATA_H
#define VK_DATA_H

#include "../render/Mesh.hpp"
#include "../render/UniformBuffer.hpp"
#include "../render/Shader.hpp"
#include "../render/Texture.h"
#include "../vulkan/ZSVulkanInstance.hpp"
#include "ZSVMA.hpp"

namespace Engine{

class _vk_Mesh : public Engine::Mesh {
private:
    VmaVkBuffer vertexBuffer;
    VmaVkBuffer indexBuffer;
public:
    void Init();
    void setMeshData(ZSVERTEX* vertices, unsigned int* indices, unsigned int vertices_num, unsigned int indices_num);
    void setMeshData(ZSVERTEX* vertices, unsigned int vertices_num);
    void Draw(VkCommandBuffer CmdBuf);
    void Draw();
    void DrawInstanced(unsigned int instances);
    void Destroy();

    _vk_Mesh();
    ~_vk_Mesh();
};

class _vk_Texture : public Engine::Texture{
private:
    VkImage mImage;
    VkImageView mImageView;
    void Transition(VmaVkBuffer temp);
    void CreateImageView(VkFormat format);
public:

    VkImageView GetImageView() { return mImageView; }
    void Init();
    //Loads texture from buffer
    bool LoadDDSTextureFromBuffer(unsigned char* data);
    //Use in rendering pipeline
    void Use(int slot);
    void Destroy();

    _vk_Texture();
    ~_vk_Texture();
};

class _vk_Texture3D : public Engine::Texture3D{
public:

    void Init();
    bool pushTextureBuffer(int index, unsigned char* data);
    //Use in rendering pipeline
    void Use(int slot);
    void Destroy();

    _vk_Texture3D();
    ~_vk_Texture3D();
};

class _vk_UniformBuffer : public UniformBuffer{
private:
    VmaVkBuffer mVkBuffer;
public:
    void init(unsigned int slot, unsigned int size, bool CreateCpuBuffer = false);
    void writeData(unsigned int offset, unsigned int size, void* data);
    void bind();
    void updateBufferedData();
    void Destroy();
    VkBuffer GetBuffer() { return mVkBuffer.Buffer; }

    _vk_UniformBuffer();
    ~_vk_UniformBuffer();
};

class _vk_Shader : public Engine::Shader{
private:

public:

    VkShaderModule vertexShader;
    VkShaderModule fragmentShader;
    VkShaderModule geometryShader;

    bool readBinaryShaderFile(std::string path, char** result, size_t& size);

    bool compileFromFile(std::string VSpath, std::string FSpath, std::string GSpath = "");
    
    void Use();
    void Destroy();

    _vk_Shader();
    ~_vk_Shader();
};

}


#endif // VK_DATA_H
