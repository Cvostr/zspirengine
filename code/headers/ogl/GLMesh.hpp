#pragma once

#include "../render/Mesh.hpp"

namespace Engine {
    class glMesh : public Engine::Mesh {
    private:
        unsigned int meshVAO;
        unsigned int meshVBO;
        unsigned int meshEBO;
    public:

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

        glMesh();
        ~glMesh();
    };
}