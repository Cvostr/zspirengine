#include "../../headers/engine/Resources.hpp"
#include "../../headers/game.h"

extern ZSGAME_DATA* game_data;

Engine::MaterialResource::MaterialResource():
    material(nullptr)
{
    this->resource_type = RESOURCE_TYPE_MATERIAL;
    loadInstantly = true;
}

void Engine::MaterialResource::load() {
    if (this->resource_state == RESOURCE_STATE::STATE_NOT_LOADED) {
        request = new Engine::Loader::LoadRequest;
        request->offset = this->offset;
        request->size = this->size;
        request->file_path = this->blob_path;

        std::string absolute = "";
        loadImmideately(request, &absolute);
        this->resource_state = RESOURCE_STATE::STATE_LOADED;
        this->material->loadFromBuffer(reinterpret_cast<char*>(request->data), request->size);
        this->material->file_path = absolute;
        delete[] request->data;
        delete request;
    }
}

Engine::MaterialResource* Engine::ResourceManager::getMaterialByLabel(std::string label) {
    for (unsigned int res = 0; res < this->resources.size(); res++) {
        ZsResource* resource_ptr = this->resources[res];
        if (resource_ptr->resource_type == RESOURCE_TYPE_MATERIAL && resource_ptr->resource_label.compare(label) == 0)
            return static_cast<MaterialResource*>(resource_ptr);
    }
    return nullptr;
}

void Engine::ResourceManager::saveMaterials() {
    for (unsigned int res = 0; res < this->resources.size(); res++) {
        ZsResource* resource_ptr = this->resources[res];
        if (resource_ptr->resource_type == RESOURCE_TYPE_MATERIAL)
            static_cast<MaterialResource*>(resource_ptr)->material->saveToFile();
    }
}