#include "../../headers/engine/resources.h"
#include <fstream>

Engine::ZsResource::ZsResource(){
    this->resource_type = TYPE_NONE;
}

Engine::ResourceManager::ResourceManager(){
    MeshResource* plane_resource = new MeshResource;

    plane_resource->rel_path = "@plane";
    plane_resource->resource_label = plane_resource->rel_path;
    plane_resource->mesh_ptr = Engine::getPlaneMesh2D();
    this->resources.push_back(plane_resource);
}

Engine::TextureResource::TextureResource(){
    this->resource_type = TYPE_TEXTURE;
}

Engine::MeshResource::MeshResource(){
    this->resource_type = TYPE_MESH;
}

Engine::AudioResource::AudioResource(){
    this->resource_type = TYPE_AUDIO;
}

void Engine::ResourceManager::loadResourcesTable(std::string resmap_path){
    std::ifstream file_stream;
    file_stream.open(resmap_path, std::ifstream::binary); //open resources map file

    std::string prefix;

    while(!file_stream.eof()){
        file_stream >> prefix;

        if(prefix.compare("_END") == 0){
            return;
        }

        if(prefix.compare("entry") == 0){
            ZsResource resource;
            //read relative path
            file_stream >> resource.rel_path >> resource.resource_label;

            file_stream.seekg(1, std::ofstream::cur); //Skip space
            //start byte
            file_stream.read(reinterpret_cast<char*>(&resource.offset), sizeof(int64_t));
            //resource size
            file_stream.read(reinterpret_cast<char*>(&resource.size), sizeof(int));
            //reading resource type
            file_stream.read(reinterpret_cast<char*>(&resource.resource_type), sizeof(RESTYPE));
            file_stream.seekg(1, std::ofstream::cur); //Skip space

            ZsResource* resource_ptr = nullptr;

            switch(resource.resource_type){
                case TYPE_TEXTURE:{
                    resource_ptr = new Engine::TextureResource;

                    Engine::TextureResource* texture_ptr = static_cast<Engine::TextureResource*>(resource_ptr);
                    texture_ptr->texture_ptr = new Engine::Texture;
                    break;
                }
                case TYPE_MESH:{
                    resource_ptr = new MeshResource;

                    Engine::MeshResource* mesh_ptr = static_cast<Engine::MeshResource*>(resource_ptr);
                    mesh_ptr->mesh_ptr = new Engine::Mesh;
                    break;
                }
                case TYPE_AUDIO:{
                    resource_ptr = new Engine::AudioResource;
                    break;
                }
                case TYPE_MATERIAL:{
                    resource_ptr = new Engine::AudioResource;
                    break;
                }
            }
            resource_ptr->offset = resource.offset;
            resource_ptr->rel_path = resource.rel_path;
            resource_ptr->size = resource.size;

            this->resources.push_back(resource_ptr);
        }
    }
    file_stream.close();
}

Engine::TextureResource* Engine::ResourceManager::getTextureByLabel(std::string label){
    for(unsigned int res = 0; res < this->resources.size(); res ++){
        ZsResource* resource_ptr = this->resources[res];
        if(resource_ptr->resource_type == TYPE_TEXTURE && resource_ptr->rel_path.compare(label) == 0)
            return static_cast<TextureResource*>(resource_ptr);
    }
    return nullptr;
}

Engine::MeshResource* Engine::ResourceManager::getMeshByLabel(std::string label){
    for(unsigned int res = 0; res < this->resources.size(); res ++){
        ZsResource* resource_ptr = this->resources[res];
        if(resource_ptr->resource_type == TYPE_MESH && resource_ptr->resource_label.compare(label) == 0)
            return static_cast<MeshResource*>(resource_ptr);
    }
    return nullptr;
}