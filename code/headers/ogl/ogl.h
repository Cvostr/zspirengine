#ifndef I_OGL
#define I_OGL

#include "../render/Mesh.hpp"
#include "../render/UniformBuffer.hpp"
#include "../render/Shader.hpp"
#include "../render/Texture.h"

namespace Engine{

class _ogl_Mesh : public Engine::Mesh {
public:

    unsigned int meshVAO;
    unsigned int meshVBO;
    unsigned int meshEBO;

    void Init();
    void setMeshData(ZSVERTEX* vertices, unsigned int* indices, unsigned int vertices_num, unsigned int indices_num);
    void setMeshData(ZSVERTEX* vertices, unsigned int vertices_num);
    void setMeshData(void* vertices, int vertSize, unsigned int vertices_num);
    void setMeshData(void* vertices, int vertSize, unsigned int* indices, unsigned int vertices_num, unsigned int indices_num);

    void setMeshOffsets();
    void Draw();
    void DrawInstanced(unsigned int instances);
    void DrawLines();
    void Destroy();
    void _glVertexAttribPointer(int index, int elems_count, int format, int normalized, int structSize, void* offset);
    void _glVertexAttribIPointer(int index, int elems_count, int format, int structSize, void* offset);

    _ogl_Mesh();
    ~_ogl_Mesh();
};

class _ogl_Texture : public Texture{
public:

    unsigned int TEXTURE_ID;

    //Only for OGL : initialize texture in GL
    void Init();
    //Loads texture from buffer
    bool LoadDDSTextureFromBuffer(unsigned char* data);
    bool LoadPNGTextureFromBuffer(unsigned char* data, int size);
    bool LoadTextureFromBufferUByte(unsigned char* data, int Width, int Height, TextureFormat format);
    //Use in rendering pipeline
    void Use(int slot);
    void Destroy();

    _ogl_Texture();
    ~_ogl_Texture();
};

class _ogl_Texture3D : public Texture3D{
public:
    unsigned int TEXTURE_ID;

    void Init();
    bool pushTextureBuffer(int index, unsigned char* data);
    //Use in rendering pipeline
    void Use(int slot);
    void Destroy();

    _ogl_Texture3D();
    ~_ogl_Texture3D();
};


class _ogl_UniformBuffer : public UniformBuffer{
private:
    //openGL integer ID
    unsigned int buffer_id;
public:
    void init(unsigned int slot, unsigned int size);
    void writeData(unsigned int offset, unsigned int size, void* data);
    void bind();
    void Destroy();

    _ogl_UniformBuffer();
    ~_ogl_UniformBuffer();
};

class _ogl_Shader : public Engine::Shader{
private:
    unsigned int mShaderID;
public:
    void GLcheckCompileErrors(unsigned int shader, const char* type, const char* filepath = nullptr);
    bool readShaderFile(const char* path, char** result, size_t& size);
    bool compileFromFile(std::string VSpath, std::string FSpath, std::string GSpath = "");
    bool compileFromStr(const char* _VS, const char* _FS, const char* _GS = nullptr);
    void setUniformBufferBinding(const char* UB_NAME, unsigned int binding);
    void Use();
    void Destroy();

    _ogl_Shader();
    ~_ogl_Shader();
};

}

#endif
