#include "../../headers/engine/resources.h"

Engine::AnimationResource::AnimationResource() {
    this->resource_type = RESOURCE_TYPE_ANIMATION;
    animation_ptr = nullptr;
}

Engine::AnimationResource::~AnimationResource() {
    delete animation_ptr;
}

void Engine::AnimationResource::load() {
    if (this->resource_state == RESOURCE_STATE::STATE_NOT_LOADED) {
        request = new Engine::Loader::LoadRequest;
        request->offset = this->offset;
        request->size = this->size;
        request->file_path = this->blob_path;
        Engine::Loader::queryLoadingRequest(request);
        this->resource_state = RESOURCE_STATE::STATE_LOADING_PROCESS;
    }
    if (this->resource_state == RESOURCE_STATE::STATE_LOADING_PROCESS) {
        if (this->request->done) {
            ZS3M::ImportedAnimationFile iaf;
            iaf.loadFromBuffer(reinterpret_cast<char*>(request->data), request->size);
            this->animation_ptr = iaf.anim_ptr;

            delete[] request->data;
            delete this->request;
            this->resource_state = RESOURCE_STATE::STATE_LOADED;
            this->resource_label = iaf.anim_ptr->name;
        }
    }

}
Engine::AnimationResource* Engine::ResourceManager::getAnimationByLabel(std::string label) {
    for (unsigned int res = 0; res < this->resources.size(); res++) {
        ZsResource* resource_ptr = this->resources[res];
        if (resource_ptr->resource_type == RESOURCE_TYPE_ANIMATION && resource_ptr->resource_label.compare(label) == 0)
            return static_cast<AnimationResource*>(resource_ptr);
    }
    return nullptr;
}