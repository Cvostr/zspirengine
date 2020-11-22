#pragma once

#include <string>
#include <vector>

#include "../render/Texture.h"
#include "../render/Mesh.hpp"
#include "../render/Material.hpp"
#include "../render/Animation.hpp"
#include "../misc/zs3m-master.h"
#include "../misc/oal_manager.h"
#include "../misc/GlyphManager.h"
#include "BackgroundLoader.hpp"
#include "EngineComponent.h"

#include "../ogl/ogl.h"
#include "../vulkan/vk_data.h"

enum RESOURCE_TYPE {RESOURCE_TYPE_NONE,
                    RESOURCE_TYPE_TEXTURE,
                    RESOURCE_TYPE_MESH,
                    RESOURCE_TYPE_AUDIO,
                    RESOURCE_TYPE_MATERIAL,
                    RESOURCE_TYPE_SCRIPT,
                    RESOURCE_TYPE_ANIMATION,
                    RESOURCE_TYPE_FONT,
                    RESOURCE_TYPE_LOCALIZED_STR,
                    RESOURCE_TYPE_SCENE,
                    RESOURCE_TYPE_FILE = 1000};

enum class RESOURCE_STATE {STATE_LOADED, STATE_NOT_LOADED, STATE_LOADING_PROCESS, RESOURCE_LOAD_FAILED};

namespace Engine {

class ZsResource{
public:
    Engine::Loader::LoadRequest* request;

    RESOURCE_TYPE resource_type; //Type of resource
    RESOURCE_STATE resource_state; //State of resource

    std::string rel_path; //Path to resource
    std::string resource_label; //Label of resource

    //Relative path to blob file
    std::string blob_path;
    //Offset of file from blob start
    uint64_t offset;
    //Size of file in blob
    unsigned int size;

    bool loadInstantly;

    virtual void Release();
    virtual void load();

    ZsResource();
    virtual ~ZsResource();
};

class TextureResource;
class MeshResource;
class AudioResource;
class ScriptResource;
class MaterialResource;
class AnimationResource;
class GlyphResource;

class ResourceManager : public IEngineComponent {
private:
    std::vector<ZsResource*> resources;
public:
    void loadResourcesTable(std::string resmap_path);
    void loadResourcesTableFromMem(char* data, unsigned int size);
    void clear();
    //Push manually configured ZsResource from outside
    void pushResource(ZsResource* resource);
    ZsResource* getResourceByIndex(unsigned int index);
    unsigned int getResourcesSize();

    void reloadScripts();

    TextureResource* getTextureByLabel(std::string label);
    MeshResource* getMeshByLabel(std::string label);
    AudioResource* getAudioByLabel(std::string label);
    ScriptResource* getScriptByLabel(std::string label);
    MaterialResource* getMaterialByLabel(std::string label);
    AnimationResource* getAnimationByLabel(std::string label);
    GlyphResource* getFontByLabel(std::string label);

    template<typename T>
    T* getResource(std::string label){
        unsigned int res = static_cast<unsigned int>(this->resources.size());
        for(unsigned int res_i = 0; res_i < res; res_i ++){
            auto property_ptr = this->resources[res_i];
            if(typeid( *property_ptr) == typeid(T) && !property_ptr->resource_label.compare(label)){ //If object already has one
                return static_cast<T*>(property_ptr); //return it
            }
        }
        return nullptr;
    }

    Engine::ZsResource* getResource(std::string label);

    ResourceManager();
    ~ResourceManager();
};

class TextureResource : public ZsResource{
public:
    Engine::Texture* texture_ptr;

    void Use(int slot);
    void Release();

    TextureResource();
    ~TextureResource();
};

class MeshResource : public ZsResource{
public:
    Engine::Mesh* mesh_ptr;

    void Draw();
    void DrawInstanced(unsigned int inst_num);
    void Release();

    MeshResource();
    ~MeshResource();
};

class AudioResource : public ZsResource{
public:
    Engine::SoundBuffer* buffer;

    void load();
    void Release();

    AudioResource();
    ~AudioResource();
};

class ScriptResource : public ZsResource{
public:
    bool hasError;
    std::string script_content;

    void load();

    ScriptResource();
};

class MaterialResource : public ZsResource{
public:
    Material* material;

    void load();

    MaterialResource();
};

class AnimationResource : public ZsResource{
public:
    Engine::Animation* animation_ptr;

    void load();

    AnimationResource();
    ~AnimationResource();
};

class GlyphResource : public ZsResource{
public:
    GlyphFontContainer* font_ptr;

    void load();

    GlyphResource();
    ~GlyphResource();
};

}