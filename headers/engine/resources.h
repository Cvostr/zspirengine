#ifndef zs_resources
#define zs_resources

#include <string>
#include <vector>

enum RESTYPE {TYPE_TEXTURE, TYPE_MESH, TYPE_AUDIO, TYPE_MATERIAL};

class ZsResource{
public:
    RESTYPE resource_type; //Type of resource

    std::string rel_path; //Path to resource
};

class ResourceManager{
private:
    std::vector<ZsResource> resources;
public:
    //void lookForResources(std::string rootDir);
};

class TextureResource : public ZsResource{
public:
    TextureResource();
};

class MeshResource : public ZsResource{
public:
    MeshResource();
};

#endif
