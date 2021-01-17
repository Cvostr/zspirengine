#include "../../headers/engine/Resources.hpp"

void Engine::SceneResource::load() {
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

Engine::SceneResource::SceneResource() {
	this->resource_type = RESOURCE_TYPE::RESOURCE_TYPE_SCENE;
}

Engine::SceneResource::~SceneResource() {

}