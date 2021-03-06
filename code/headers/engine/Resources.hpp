#pragma once

#include <string>
#include <vector>

#include "../render/Framebuffer.hpp"
#include "../render/Texture.h"
#include "../render/Mesh.hpp"
#include "../render/Material.hpp"
#include "../render/Animation.hpp"
#include "../misc/zs3m-master.h"
#include "../audio/SoundSource.hpp"
#include "../misc/GlyphManager.h"
#include "LocaleStringsFile.h"
#include "BackgroundLoader.hpp"
#include "EngineComponent.h"

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
                    RESOURCE_TYPE_PREFAB,
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
class PrefabResource;
class LocalizedStringResource;

class ResourceManager : public IEngineComponent {
private:
    std::vector<ZsResource*> resources;
    std::vector<LocalizedStringResource*> mLSResources;
public:
    void loadResourcesTable(std::string resmap_path);
    void loadResourcesTableFromMem(char* data, unsigned int size);
    void clear();
    //Push manually configured ZsResource from outside
    void pushResource(ZsResource* resource);
    ZsResource* getResourceByIndex(unsigned int index);
    unsigned int getResourcesSize();
    unsigned int getActiveResourcesCount();

    void reloadScripts();
    void saveMaterials();

    TextureResource* getTextureByLabel(std::string label);
    MeshResource* getMeshByLabel(std::string label);
    AudioResource* getAudioByLabel(std::string label);
    ScriptResource* getScriptByLabel(std::string label);
    MaterialResource* getMaterialByLabel(std::string label);
    AnimationResource* getAnimationByLabel(std::string label);
    GlyphResource* getFontByLabel(std::string label);
    PrefabResource* getPrefabByLabel(std::string label);

    ZSPIRE::LocString* getStringByStrId(std::string strid);

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
    Engine::ZsResource* getResourceByRelPath(std::string label);
    void DeleteResource(Engine::ZsResource* resource);

    ResourceManager();
    ~ResourceManager();
};

class TextureResource : public ZsResource{
public:
    Engine::Texture* texture_ptr;
    bool IsRenderTarget;

    void load();
    void loadAndWait();
    void Use(int slot);
    void Release();

    TextureResource();
    ~TextureResource();
};

class MeshResource : public ZsResource{
public:
    Engine::Mesh* mesh_ptr;

    void load();
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
    std::string script_content;
    std::string ClassName;

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

class PrefabResource : public ZsResource {
public:

    unsigned char* data;
    void load();

    PrefabResource();
    ~PrefabResource();
};

class SceneResource : public ZsResource {
public:

    unsigned char* data;
    void load();

    SceneResource();
    ~SceneResource();
};

class LocalizedStringResource : public ZsResource {
private:
    unsigned char* data;
public:
    ZSPIRE::LocaleStringsFile* mLSFile;

    void load();
    void Release();
    void Reload();

    LocalizedStringResource();
    ~LocalizedStringResource();
};

}