#ifndef go_properties
#define go_properties

#include "World.h"
namespace Engine {

class MeshProperty : public GameObjectProperty{
public:
    std::string resource_relpath; //Relative path to resource
    //ZSPIRE::Mesh* mesh_ptr; //Pointer to mesh

    void updateMeshPtr(); //Updates pointer according to resource_relpath
    //void copyTo(GameObjectProperty* dest);
    MeshProperty();
};

}

#endif
