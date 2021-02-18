#include <engine/Resources.hpp>

Engine::TextureResource::TextureResource() :
    texture_ptr(nullptr)
{
    this->resource_type = RESOURCE_TYPE_TEXTURE;
}

Engine::TextureResource::~TextureResource() {
    delete texture_ptr;
}

void Engine::TextureResource::load() {
    //Otherwise perform texture loading
    if (this->resource_state == RESOURCE_STATE::STATE_NOT_LOADED) {
        //Create loading request
        request = new Engine::Loader::LoadRequest;
        //Set offset of resource in blob
        request->offset = this->offset;
        //Set size of resource in blob
        request->size = this->size;
        request->file_path = this->blob_path;
        //Send request to queue
        Engine::Loader::queryLoadingRequest(request);
        this->resource_state = RESOURCE_STATE::STATE_LOADING_PROCESS;
    }
    //Check, if texture reading process finished
    if (this->resource_state == RESOURCE_STATE::STATE_LOADING_PROCESS) {
        if (this->request->done == true) {
            this->texture_ptr->LoadFromBuffer(request->data, (uint32_t)request->size);
            //Delete read data
            delete[] request->data;
            //Delete request
            delete this->request;
            //Set state to "Loaded"
            this->resource_state = RESOURCE_STATE::STATE_LOADED;
        }
    }
}

void Engine::TextureResource::loadAndWait() {
    if (this->resource_state == RESOURCE_STATE::STATE_NOT_LOADED) {
        load();
        while (this->resource_state != RESOURCE_STATE::STATE_LOADED) {
            load();
        }
    }
}

void Engine::TextureResource::Use(int slot) {
    //Check, if texture already loaded
    if (this->resource_state == RESOURCE_STATE::STATE_LOADED)
        //If loaded, just use it in slot
        this->texture_ptr->Use(slot);
    load();
}

void Engine::TextureResource::Release() {
    if (this->resource_state == RESOURCE_STATE::STATE_LOADED) {
        this->texture_ptr->Destroy();
        this->resource_state = RESOURCE_STATE::STATE_NOT_LOADED;
    }
}

Engine::TextureResource* Engine::ResourceManager::getTextureByLabel(std::string label) {
    for (unsigned int res = 0; res < this->resources.size(); res++) {
        ZsResource* resource_ptr = this->resources[res];
        if (resource_ptr->resource_type == RESOURCE_TYPE_TEXTURE && resource_ptr->resource_label.compare(label) == 0)
            return static_cast<TextureResource*>(resource_ptr);
    }
    return nullptr;
}
