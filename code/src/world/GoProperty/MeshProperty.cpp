#include "../../../headers/world/go_properties.h"

extern ZSGAME_DATA* game_data;

Engine::MeshProperty::MeshProperty() {
    this->type = PROPERTY_TYPE::GO_PROPERTY_TYPE_MESH;
    mesh_ptr = nullptr;

    castShadows = true;

    this->skinning_root_node = nullptr;
    this->rootNodeStr = "@none";
}
void Engine::MeshProperty::copyTo(GameObjectProperty* dest) {
    if (dest->type != this->type) return; //if it isn't transform

    MeshProperty* _dest = static_cast<MeshProperty*>(dest);
    _dest->resource_relpath = resource_relpath;
    _dest->mesh_ptr = mesh_ptr;
    _dest->castShadows = castShadows;
    _dest->rootNodeStr = rootNodeStr;
}
void Engine::MeshProperty::updateMeshPtr() {
    this->mesh_ptr = game_data->resources->getMeshByLabel(this->resource_relpath);
}

void Engine::MeshProperty::setMeshResource(MeshResource* resource) {
    this->resource_relpath = resource->resource_label;
    updateMeshPtr();
}

void Engine::MeshProperty::onRender(Engine::RenderPipeline* pipeline) {
    if (this->skinning_root_node == nullptr && mesh_ptr->mesh_ptr->hasBones())
        skinning_root_node = go_link.world_ptr->getGameObjectByLabel(this->rootNodeStr);
}

void Engine::MeshProperty::onValueChanged() {
    updateMeshPtr();
}

void Engine::MeshProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
    unsigned int offset = 1;

    while (data[offset] != ' ' && data[offset] != '\n') {
        resource_relpath += data[offset];
        offset++;
    }
    offset++;

    rootNodeStr.clear();
    //Read Root Node label
    while (data[offset] != ' ' && data[offset] != '\n') {
        rootNodeStr += data[offset];
        offset++;
    }
    //Pointer will now point to mesh resource
    updateMeshPtr();

    offset++;
    //Read castShadows bool
    memcpy(&castShadows, data + offset, sizeof(bool));
    offset += sizeof(bool);
}