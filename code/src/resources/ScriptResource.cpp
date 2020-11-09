#include "../../headers/engine/resources.h"
#include "../../headers/game.h"
#include "../../headers/Scripting/ZPScriptClass.hpp"

extern ZSGAME_DATA* game_data;

Engine::ScriptResource::ScriptResource() {
    this->resource_type = RESOURCE_TYPE_SCRIPT;
    loadInstantly = true;
}

void Engine::ScriptResource::load() {
    if (this->resource_state == RESOURCE_STATE::STATE_NOT_LOADED) {
        request = new Engine::Loader::LoadRequest;
        request->offset = this->offset;
        request->size = this->size;
        request->file_path = this->blob_path;
        loadImmideately(request);
        this->resource_state = RESOURCE_STATE::STATE_LOADED;
        //Clear string data, first
        script_content.clear();
        for (unsigned int i = 0; i < this->request->size; i++) {
            this->script_content.push_back(static_cast<char>(request->data[i]));
        }

        delete[] request->data;
        delete this->request;
    }
}

void Engine::ResourceManager::reloadScripts() {
    for (unsigned int i = 0; i < resources.size(); i++) {
        ZsResource* res = resources[i];
        if (res->resource_type == RESOURCE_TYPE_SCRIPT) {
            ScriptResource* script_res = static_cast<ScriptResource*>(res);
            res->Release();
            res->load();
        }
    }
}

Engine::ScriptResource* Engine::ResourceManager::getScriptByLabel(std::string label) {
    for (unsigned int res = 0; res < this->resources.size(); res++) {
        ZsResource* resource_ptr = this->resources[res];
        if (resource_ptr->resource_type == RESOURCE_TYPE_SCRIPT && resource_ptr->resource_label.compare(label) == 0)
            return static_cast<ScriptResource*>(resource_ptr);
    }
    return nullptr;
}