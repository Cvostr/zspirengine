#ifndef zs_resources
#define zs_resources

#include <string>
#include <vector>

#include "../render/zs-texture.h"
#include "../render/zs-mesh.h"
#include "loader.h"
#include "EngineManager.h"

enum RESTYPE {TYPE_NONE, TYPE_TEXTURE, TYPE_MESH, TYPE_AUDIO, TYPE_MATERIAL};

enum RESOURCE_STATE {STATE_LOADED, STATE_NOT_LOADED, STATE_LOADING_PROCESS};

namespace Engine {

class ZsResource{
public:
    Engine::Loader::LoadRequest* request;

    RESTYPE resource_type; //Type of resource
    RESOURCE_STATE resource_state; //State of resource

    std::string rel_path; //Path to resource
    std::string resource_label; //Label of resource

    std::string blob_path;

    uint64_t offset;
    unsigned int size;

    virtual void Release();

    ZsResource();
    virtual ~ZsResource();
};

class TextureResource;
class MeshResource;
class AudioResource;

class ResourceManager : public EngineComponentManager{
private:
    std::vector<ZsResource*> resources;
public:
    void loadResourcesTable(std::string resmap_path);
    void clear();

    TextureResource* getTextureByLabel(std::string label);
    MeshResource* getMeshByLabel(std::string label);
    AudioResource* getAudioByLabel(std::string label);

    ResourceManager();
    ~ResourceManager();
};

class TextureResource : public ZsResource{
public:
    Engine::Texture* texture_ptr;

    void Use(int slot);
    void Release();

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
