#ifndef I_OGL
#define I_OGL

#include "../render/zs-mesh.h"
#include "../render/zs-uniform-buffer.h"
#include "../render/zs-shader.h"
#include "../render/zs-texture.h"

namespace Engine{

class _ogl_Mesh : public Engine::Mesh {
public:

    unsigned int meshVAO;
    unsigned int meshVBO;
    unsigned int meshEBO;

    void Init();
    void setMeshData(ZSVERTEX* vertices, unsigned int* indices, unsigned int vertices_num, unsigned int indices_num);
    void setMeshData(ZSVERTEX* vertices, unsigned int vertices_num);
    void setMeshOffsets();
    void Draw();
    void DrawInstanced(unsigned int instances);
    void DrawLines();
    void Destroy();

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
    unsigned int shader_id;
public:
    void GLcheckCompileErrors(unsigned int shader, const char* type, const char* filepath = nullptr);
    bool readShaderFile(const char* path, char* result);
    bool compileFromFile(std::string VSpath, std::string FSpath);
    bool compileFromStr(const char* _VS, const char* _FS);
    void setUniformBufferBinding(const char* UB_NAME, unsigned int binding);
    void Use();
    void Destroy();

    _ogl_Shader();
    ~_ogl_Shader();
};

}

#endif