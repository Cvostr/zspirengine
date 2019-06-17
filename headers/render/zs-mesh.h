#ifndef zs_mesh
#define zs_mesh

#include "zs-math.h"
#include <string>
#include <GL/glew.h>

#define NO_INDICES 0

namespace Engine {

    class Mesh {
    public:
        bool alive; //Non destroyed
        std::string label;

        unsigned int meshVAO;
        unsigned int meshVBO;
        unsigned int meshEBO;

        unsigned int vertices_num;
        unsigned int indices_num;

        void Init();
        void setMeshData(ZSVERTEX* vertices, unsigned int* indices, unsigned int vertices_num, unsigned int indices_num);
        void setMeshData(ZSVERTEX* vertices, unsigned int vertices_num);
        void Draw();
        void DrawLines();
        void Destroy();

        Mesh();
    };

    void setupDefaultMeshes();
    void freeDefaultMeshes();

    Mesh* getPlaneMesh2D();
    Mesh* getUiSpriteMesh2D();
    Mesh* getIsoTileMesh2D();
    Mesh* getCubeMesh3D();

}


#endif
