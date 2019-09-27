#ifndef I_OGL
#define I_OGL

#include "../render/zs-mesh.h"
#include "../render/zs-uniform-buffer.h"
#include "../render/zs-shader.h"

namespace Engine{

class _ogl_Mesh : public Engine::Mesh {
public:

    unsigned int meshVAO;
    unsigned int meshVBO;
    unsigned int meshEBO;

    void Init();
    void setMeshData(ZSVERTEX* vertices, unsigned int* indices, unsigned int vertices_num, unsigned int indices_num);
    void setMeshData(ZSVERTEX* vertices, unsigned int vertices_num);
    void Draw();
    void Destroy();

    _ogl_Mesh();
    ~_ogl_Mesh();
};

class _ogl_UniformBuffer : public UniformBuffer{
private:
    //openGL integer ID
    unsigned int buffer_id;
public:
    void init(int slot, int size);
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
    void Use();
    void Destroy();

    _ogl_Shader();
    ~_ogl_Shader();
};

}

#endif
