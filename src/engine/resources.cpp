#include "../../headers/engine/resources.h"
#include <fstream>

ZsResource::ZsResource(){
    this->resource_type = TYPE_NONE;
}

TextureResource::TextureResource(){
    this->resource_type = TYPE_TEXTURE;
}

MeshResource::MeshResource(){
    this->resource_type = TYPE_MESH;
}

AudioResource::AudioResource(){
    this->resource_type = TYPE_AUDIO;
}

void ResourceManager::loadResourcesTable(std::string resmap_path){
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

            file_stream >> resource.rel_path;

            //if(resource.rel_path == "")

            file_stream.seekg(1, std::ofstream::cur); //Skip space
            file_stream.read(reinterpret_cast<char*>(&resource.offset), sizeof(int64_t));
            file_stream.read(reinterpret_cast<char*>(&resource.size), sizeof(int));
            file_stream.read(reinterpret_cast<char*>(&resource.resource_type), sizeof(RESTYPE));
            file_stream.seekg(1, std::ofstream::cur); //Skip space

            ZsResource* resource_ptr;

            switch(resource.resource_type){
                case TYPE_TEXTURE:{
                    resource_ptr = new TextureResource;
                    break;
                }
                case TYPE_MESH:{
                    resource_ptr = new MeshResource;
                    break;
                }
                case TYPE_AUDIO:{
                    resource_ptr = new AudioResource;
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

TextureResource* ResourceManager::getTextureByLabel(std::string label){
    for(unsigned int res = 0; res < this->resources.size(); res ++){
        ZsResource* resource_ptr = this->resources[res];
        if(resource_ptr->resource_type == TYPE_TEXTURE && resource_ptr->rel_path.compare(label) == 0)
            return static_cast<TextureResource*>(resource_ptr);
    }
}
