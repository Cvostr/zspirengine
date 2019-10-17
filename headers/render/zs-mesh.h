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

        int vertices_num;
        int indices_num;

        virtual void Init();
        virtual void setMeshData(ZSVERTEX* vertices, unsigned int* indices, unsigned int vertices_num, unsigned int indices_num);
        virtual void setMeshData(ZSVERTEX* vertices, unsigned int vertices_num);
        virtual void Draw();
        virtual void Destroy();

        Mesh();
        virtual ~Mesh();
    };

    void setupDefaultMeshes();
    void freeDefaultMeshes();

    Mesh* getPlaneMesh2D();
    Mesh* getUiSpriteMesh2D();
    Mesh* getIsoTileMesh2D();
    Mesh* getCubeMesh3D();

}


#endif
