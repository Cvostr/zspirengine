#ifndef zs_resources
#define zs_resources

#include <string>
#include <vector>

#include "../render/zs-mesh.h"

enum RESTYPE {TYPE_NONE, TYPE_TEXTURE, TYPE_MESH, TYPE_AUDIO, TYPE_MATERIAL};

namespace Engine {

class ZsResource{
public:
    RESTYPE resource_type; //Type of resource

    std::string rel_path; //Path to resource
    std::string resource_label;

    int64_t offset;
    int size;

    ZsResource();
};

class TextureResource;
class MeshResource;

class ResourceManager{
private:
    std::vector<ZsResource*> resources;
public:
    void loadResourcesTable(std::string resmap_path);

    TextureResource* getTextureByLabel(std::string label);
    MeshResource* getMeshByLabel(std::string label);

    ResourceManager();
};

class TextureResource : public ZsResource{
public:
    TextureResource();
};

class MeshResource : public ZsResource{
public:
    Engine::Mesh* mesh_ptr;

    MeshResource();
};

class AudioResource : public ZsResource{
public:
    AudioResource();
};

}

#endif
