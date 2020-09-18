#include "../../headers/engine/resources.h"
#include <fstream>
#include <iostream>
#include "../../headers/engine.h"
#include "../../headers/game.h"
#include "../../headers/misc/misc.h"

extern ZSpireEngine* engine_ptr;
extern Material* default3dmat;
extern Material* defaultTerrainMat;
extern ZSGAME_DATA* game_data;

Engine::ZsResource::ZsResource(){
    this->resource_type = RESOURCE_TYPE_NONE;
    this->resource_state = RESOURCE_STATE::STATE_NOT_LOADED;
    size = 0;
    offset = 0;
    loadInstantly = false;
}

Engine::ZsResource::~ZsResource(){

}

void Engine::ZsResource::Release(){
    this->resource_state = RESOURCE_STATE::STATE_NOT_LOADED;
}

void Engine::ZsResource::load(){

}

Engine::ResourceManager::ResourceManager(){
    MeshResource* plane_resource = new MeshResource;
    plane_resource->resource_state = RESOURCE_STATE::STATE_LOADED;
    plane_resource->rel_path = "@plane";
    plane_resource->resource_label = plane_resource->rel_path;
    plane_resource->mesh_ptr = Engine::getPlaneMesh2D();
    this->resources.push_back(plane_resource);

    MeshResource* isotile_resource = new MeshResource;
    isotile_resource->resource_state = RESOURCE_STATE::STATE_LOADED;
    isotile_resource->rel_path = "@isotile";
    isotile_resource->resource_label = isotile_resource->rel_path;
    isotile_resource->mesh_ptr = Engine::getIsoTileMesh2D();
    this->resources.push_back(isotile_resource);

    MeshResource* cube_resource = new MeshResource;
    cube_resource->resource_state = RESOURCE_STATE::STATE_LOADED;
    cube_resource->rel_path = "@cube";
    cube_resource->resource_label = cube_resource->rel_path;
    cube_resource->mesh_ptr = Engine::getCubeMesh3D();
    this->resources.push_back(cube_resource);

    MeshResource* sphere_resource = new MeshResource;
    sphere_resource->resource_state = RESOURCE_STATE::STATE_LOADED;
    sphere_resource->rel_path = "@sphere";
    sphere_resource->resource_label = sphere_resource->rel_path;
    sphere_resource->mesh_ptr = Engine::getSphereMesh();
    this->resources.push_back(sphere_resource);

    //This material should be created only in 3D projects
    if(engine_ptr->desc->game_perspective == PERSP_3D){
        MaterialResource* material3d_resource = new MaterialResource;
        material3d_resource->resource_state = RESOURCE_STATE::STATE_LOADED;
        material3d_resource->rel_path = "@default";
        material3d_resource->resource_label = material3d_resource->rel_path;
        material3d_resource->material = default3dmat;
        this->resources.push_back(material3d_resource);

        MaterialResource* materialTerrain_resource = new MaterialResource;
        materialTerrain_resource->resource_state = RESOURCE_STATE::STATE_LOADED;
        materialTerrain_resource->rel_path = "@defaultHeightmap";
        materialTerrain_resource->resource_label = materialTerrain_resource->rel_path;
        materialTerrain_resource->material = defaultTerrainMat;
        this->resources.push_back(materialTerrain_resource);

    }
}

void Engine::ResourceManager::clear(){
    for(unsigned int i = 0; i < this->resources.size(); i ++){
        auto res_ptr = this->resources[i];
        res_ptr->Release();
        delete res_ptr;
    }
    this->resources.clear();
}

Engine::ResourceManager::~ResourceManager(){
    clear();
}

//Push manually configured ZsResource from outside
void Engine::ResourceManager::pushResource(ZsResource* resource){
    switch(resource->resource_type){
        case RESOURCE_TYPE_NONE:{
            break;
        }
        case RESOURCE_TYPE_FILE:{
            break;
        }
        case RESOURCE_TYPE_TEXTURE:{
            Engine::TextureResource* texture_ptr = static_cast<Engine::TextureResource*>(resource);
            texture_ptr->texture_ptr = allocTexture();
            break;
        }
        case RESOURCE_TYPE_MESH:{

            Engine::MeshResource* mesh_ptr = static_cast<Engine::MeshResource*>(resource);
            mesh_ptr->mesh_ptr = allocateMesh();
            break;
        }
        case RESOURCE_TYPE_AUDIO:{
            break;
        }
        case RESOURCE_TYPE_SCRIPT:{
            break;
        }
        case RESOURCE_TYPE_MATERIAL:{
            Engine::MaterialResource* mat_ptr = static_cast<Engine::MaterialResource*>(resource);
            mat_ptr->material = new Material;
            break;
        }
        case RESOURCE_TYPE_ANIMATION:{
            //static_cast<Engine::AnimationResource*>(resource)->animation_ptr = new Engine::Animation;
            break;
        }
        case RESOURCE_TYPE_FONT:{

            break;
        }
    }

    if(resource->loadInstantly)
        (resource)->load();

    this->resources.push_back(resource);
}

Engine::ZsResource* Engine::ResourceManager::getResource(std::string label) {
    unsigned int res = static_cast<unsigned int>(this->resources.size());
    for (unsigned int res_i = 0; res_i < res; res_i++) {
        auto property_ptr = this->resources[res_i];
        if (!property_ptr->resource_label.compare(label)) { //If object already has one
            return (property_ptr); //return it
        }
    }
    return nullptr;
}

void Engine::ResourceManager::loadResourcesTableFromMem(char* data, unsigned int size) {
    std::string prefix;
    unsigned int iter = 0;

    while (true) {
        prefix.clear();
        //Read prefix
        if (iter >= size)
            break;
        while (data[iter] != ' ' && data[iter] != '\n') {
            if (data[iter] != '\0')
                prefix += data[iter];
            iter++;
        }

        if (prefix.compare("_END") == 0) { //If end reached
            break; //Then end this infinity loop
        }
        if (prefix.compare("entry") == 0) { //If end reached
            ZsResource resource;
            iter++;

            readString(resource.rel_path, data, iter);

            readString(resource.resource_label, data, iter);

            readString(resource.blob_path, data, iter);

            readBinaryValue(&resource.offset, data + iter, iter);
            readBinaryValue(&resource.size, data + iter, iter);
            readBinaryValue(&resource.resource_type, data + iter, iter);
            iter++;

            ZsResource* resource_ptr = nullptr;

            switch (resource.resource_type) {
            default: {
                resource_ptr = new ZsResource;
                break;
            }
            case RESOURCE_TYPE_FILE: {
                break;
            }
            case RESOURCE_TYPE_TEXTURE: {
                resource_ptr = new Engine::TextureResource;

                Engine::TextureResource* texture_ptr = static_cast<Engine::TextureResource*>(resource_ptr);
                texture_ptr->texture_ptr = allocTexture();
                break;
            }
            case RESOURCE_TYPE_MESH: {
                resource_ptr = new MeshResource;

                Engine::MeshResource* mesh_ptr = static_cast<Engine::MeshResource*>(resource_ptr);
                mesh_ptr->mesh_ptr = allocateMesh();
                break;
            }
            case RESOURCE_TYPE_AUDIO: {
                resource_ptr = new Engine::AudioResource;
                break;
            }
            case RESOURCE_TYPE_SCRIPT: {
                resource_ptr = new Engine::ScriptResource;
                break;
            }
            case RESOURCE_TYPE_MATERIAL: {
                resource_ptr = new Engine::MaterialResource;
                static_cast<Engine::MaterialResource*>(resource_ptr)->material = new Material;
                break;
            }
            case RESOURCE_TYPE_ANIMATION: {
                resource_ptr = new Engine::AnimationResource;
                static_cast<Engine::AnimationResource*>(resource_ptr)->animation_ptr = new Engine::Animation;
                break;
            }
            case RESOURCE_TYPE_FONT: {
                resource_ptr = new GlyphResource;

                break;
            }
            }
            resource_ptr->blob_path = resource.blob_path;
            resource_ptr->offset = resource.offset;
            resource_ptr->rel_path = resource.rel_path;
            resource_ptr->resource_label = resource.resource_label;
            resource_ptr->size = resource.size;

            if (resource_ptr->loadInstantly)
                (resource_ptr)->load();
            //Add resource to vector
            this->resources.push_back(resource_ptr);
        
        }
    }
}

void Engine::ResourceManager::loadResourcesTable(std::string resmap_path){
    std::ifstream file_stream;
    file_stream.open(resmap_path, std::ifstream::binary | std::ifstream::ate); //open resources map file

    if (!file_stream.is_open()) {
        std::cout << "Error opening Resource Table File " << resmap_path << std::endl; 
    }

    unsigned int size = static_cast<unsigned int>(file_stream.tellg());
    file_stream.seekg(0);
    char* data = new  char[size];
    file_stream.read(data, size);
    loadResourcesTableFromMem(data, size);

    delete[] data;
    file_stream.close();
}

Engine::ZsResource* Engine::ResourceManager::getResourceByIndex(unsigned int index){
    return resources[index];
}

unsigned int Engine::ResourceManager::getResourcesSize(){
    return static_cast<unsigned int>(resources.size());
}

Engine::ScriptResource::ScriptResource(){
    this->resource_type = RESOURCE_TYPE_SCRIPT;
    loadInstantly = true;
}

void Engine::ScriptResource::load(){
    if(this->resource_state == RESOURCE_STATE::STATE_NOT_LOADED){
        request = new Engine::Loader::LoadRequest;
        request->offset = this->offset;
        request->size = this->size;
        request->file_path = this->blob_path;
        loadImmideately(request);
        this->resource_state = RESOURCE_STATE::STATE_LOADED;
        //Clear string data, first
        script_content.clear();
        for(unsigned int i = 0; i < this->request->size; i ++){
            this->script_content.push_back(static_cast<char>(request->data[i]));
        }
        //size = request->size;

        delete[] request->data;
        delete this->request;
    }
}

void Engine::ResourceManager::reloadScripts() {
    for (unsigned int i = 0; i < resources.size(); i++) {
        ZsResource* res = resources[i];
        if (res->resource_type == RESOURCE_TYPE_SCRIPT) {
            res->Release();
            res->load();
        }
    }
}

Engine::ScriptResource* Engine::ResourceManager::getScriptByLabel(std::string label){
    for(unsigned int res = 0; res < this->resources.size(); res ++){
        ZsResource* resource_ptr = this->resources[res];
        if(resource_ptr->resource_type == RESOURCE_TYPE_SCRIPT && resource_ptr->resource_label.compare(label) == 0)
            return static_cast<ScriptResource*>(resource_ptr);
    }
    return nullptr;
}

Engine::Mesh* Engine::allocateMesh(unsigned int size){
    Engine::Mesh* result = nullptr;
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL32 : {
            result = new _ogl_Mesh[size];
            break;
        }
        case VULKAN : {
            result = new _vk_Mesh[size];
            break;
        }
    }
    return result;
}

Engine::Mesh* Engine::allocateMesh(){
    Engine::Mesh* result = nullptr;
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL32 : {
            result = new _ogl_Mesh;
            break;
        }
        case VULKAN : {
            result = new _vk_Mesh;
            break;
        }
    }
    return result;
}
