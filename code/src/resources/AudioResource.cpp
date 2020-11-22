#include "../../headers/engine/Resources.hpp"

Engine::AudioResource::AudioResource() {
    this->resource_type = RESOURCE_TYPE_AUDIO;
    buffer = new SoundBuffer;
}

Engine::AudioResource::~AudioResource() {
    delete buffer;
}

void Engine::AudioResource::load() {
    //if resource isn't loaded
    if (this->resource_state == RESOURCE_STATE::STATE_NOT_LOADED) {
        request = new Engine::Loader::LoadRequest;
        request->offset = this->offset;
        request->size = this->size;
        request->file_path = this->blob_path;
        //Submit loading request
        Engine::Loader::queryLoadingRequest(request);
        //Set LOADING flag to resource
        this->resource_state = RESOURCE_STATE::STATE_LOADING_PROCESS;
    }
    if (this->resource_state == RESOURCE_STATE::STATE_LOADING_PROCESS) {
        if (this->request->done) {
            this->buffer->loadBufferWAV(request->data);
            delete[] request->data;
            delete this->request;
            this->resource_state = RESOURCE_STATE::STATE_LOADED;
        }
    }
}

void Engine::AudioResource::Release() {
    if (this->resource_state == RESOURCE_STATE::STATE_LOADED) {
        this->buffer->Destroy();
        this->resource_state = RESOURCE_STATE::STATE_NOT_LOADED;
    }
}

Engine::AudioResource* Engine::ResourceManager::getAudioByLabel(std::string label) {
    for (unsigned int res = 0; res < this->resources.size(); res++) {
        ZsResource* resource_ptr = this->resources[res];
        if (resource_ptr->resource_type == RESOURCE_TYPE_AUDIO && resource_ptr->resource_label.compare(label) == 0)
            return static_cast<AudioResource*>(resource_ptr);
    }
    return nullptr;
}