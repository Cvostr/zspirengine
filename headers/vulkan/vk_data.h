#ifndef VK_DATA_H
#define VK_DATA_H

#include "../render/zs-mesh.h"
#include "../render/zs-uniform-buffer.h"

namespace Engine{

class _vk_Mesh : public Engine::Mesh {
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
    void init(int slot, int size);
    void writeData(unsigned int offset, unsigned int size, void* data);
    void bind();

    _vk_UniformBuffer();
    ~_vk_UniformBuffer();
};



}


#endif // VK_DATA_H
