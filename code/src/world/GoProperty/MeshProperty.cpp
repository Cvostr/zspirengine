#include "../../../headers/world/go_properties.h"

extern ZSGAME_DATA* game_data;

Engine::MeshProperty::MeshProperty() {
    this->type = PROPERTY_TYPE::GO_PROPERTY_TYPE_MESH;
    mesh_ptr = nullptr;

    castShadows = true;

    this->skinning_root_node = nullptr;

    this->resource_relpath = "@none";
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

    readString(resource_relpath, data, offset);
    offset++;

    rootNodeStr.clear();
    //Read Root Node label
    readString(rootNodeStr, data, offset);
    //Pointer will now point to mesh resource
    updateMeshPtr();

    offset++;
    //Read castShadows bool
    memcpy(&castShadows, data + offset, sizeof(bool));
    offset += sizeof(bool);
}

void Engine::MeshProperty::savePropertyToStream(std::ofstream* stream, GameObject* obj) {
    *stream << resource_relpath << '\0';
    if (skinning_root_node != nullptr)
        *stream << *skinning_root_node->label_ptr << '\0';
    else
        *stream << "@none" << '\0';
    stream->write(reinterpret_cast<char*>(&castShadows), sizeof(bool));
}

void Engine::MeshProperty::bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr) {
    mgr->RegisterObjectType(MESH_PROP_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);

    mgr->RegisterObjectMethod(MESH_PROP_TYPE_NAME, "void setMesh(MeshResource@)", asMETHOD(MeshProperty, setMeshResource), asCALL_THISCALL);

}