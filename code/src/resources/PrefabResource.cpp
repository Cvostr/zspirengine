#include "../../headers/engine/Resources.hpp"

Engine::PrefabResource::PrefabResource() {
	this->resource_type = RESOURCE_TYPE::RESOURCE_TYPE_PREFAB;
    loadInstantly = true;
}
Engine::PrefabResource::~PrefabResource() {
    
}

void Engine::PrefabResource::load() {
    if (this->resource_state == RESOURCE_STATE::STATE_NOT_LOADED) {
        request = new Engine::Loader::LoadRequest;
        request->offset = this->offset;
        request->size = this->size;
        request->file_path = this->blob_path;
        loadImmideately(request);
        this->resource_state = RESOURCE_STATE::STATE_LOADED;

        this->size = request->size;
        this->data = request->data;

        delete this->request;
    }
}

Engine::PrefabResource* Engine::ResourceManager::getPrefabByLabel(std::string label) {
    for (unsigned int res = 0; res < this->resources.size(); res++) {
        ZsResource* resource_ptr = this->resources[res];
        if (resource_ptr->resource_type == RESOURCE_TYPE_PREFAB && resource_ptr->resource_label.compare(label) == 0)
            return static_cast<PrefabResource*>(resource_ptr);
    }
    return nullptr;
}