#include <engine/Resources.hpp>

Engine::LocalizedStringResource::LocalizedStringResource() {
	this->resource_type = RESOURCE_TYPE::RESOURCE_TYPE_LOCALIZED_STR;
    this->loadInstantly = true;
}
Engine::LocalizedStringResource::~LocalizedStringResource() {

}

void Engine::LocalizedStringResource::load() {

    if (this->resource_state == RESOURCE_STATE::STATE_NOT_LOADED) {
        request = new Engine::Loader::LoadRequest;
        request->offset = this->offset;
        request->size = this->size;
        request->file_path = this->blob_path;
        loadImmideately(request);
        this->resource_state = RESOURCE_STATE::STATE_LOADED;

        this->size = request->size;
        this->data = request->data;

        this->mLSFile = new ZSPIRE::LocaleStringsFile((char*)request->data);

        delete this->request;
    }	
}

void Engine::LocalizedStringResource::Release() {
    if (this->resource_state == RESOURCE_STATE::STATE_LOADED) {
        delete mLSFile;
        this->resource_state = RESOURCE_STATE::STATE_NOT_LOADED;
    }
}

void Engine::LocalizedStringResource::Reload() {

}