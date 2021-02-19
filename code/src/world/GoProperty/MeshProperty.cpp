#include <world/ObjectsComponents/MeshComponent.hpp>
#include <game.h>

extern ZSGAME_DATA* game_data;

Engine::MeshProperty::MeshProperty() :
    mesh_ptr(nullptr),
    castShadows(true),
    resource_relpath("@none"),
    rootNodeStr("@none"),
    skinning_root_node(nullptr)
{
    this->type = PROPERTY_TYPE::GO_PROPERTY_TYPE_MESH;
}
void Engine::MeshProperty::copyTo(IGameObjectComponent* dest) {
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

void Engine::MeshProperty::updateRootNodePtr() {
    skinning_root_node = go_link.world_ptr->getGameObjectByLabel(this->rootNodeStr);
}

void Engine::MeshProperty::setMeshResource(MeshResource* resource) {
    this->resource_relpath = resource->resource_label;
    updateMeshPtr();
}

bool Engine::MeshProperty::HasBones() {
    if (mesh_ptr != nullptr)
        if (mesh_ptr->mesh_ptr != nullptr)
            if (mesh_ptr->mesh_ptr->hasBones())
                return true;
        else
            return false;
    else
        return false;
    return false;
}

void Engine::MeshProperty::onPreRender(Engine::Renderer* pipeline) {
    if (this->skinning_root_node == nullptr && HasBones())
        updateRootNodePtr();
}

void Engine::MeshProperty::onValueChanged() {
    updateMeshPtr();
}

void Engine::MeshProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
    unsigned int offset = 1;
    //Read Mesh label
    readString(resource_relpath, data, offset);
    //Read Root Node label
    readString(rootNodeStr, data, offset);
    //Pointer will now point to mesh resource
    updateMeshPtr();
    //Read castShadows bool
    readBinaryValue(&castShadows, data + offset, offset);
}

void Engine::MeshProperty::savePropertyToStream(ZsStream* stream, GameObject* obj) {
    stream->writeString(resource_relpath);
    updateRootNodePtr(); //update node pointer
    if (skinning_root_node != nullptr)
        stream->writeString(skinning_root_node->getLabel());
    else
        stream->writeString("@none");

    stream->writeBinaryValue(&castShadows);
}

void Engine::MeshProperty::bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr) {
    mgr->RegisterObjectType(MESH_PROP_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);

    mgr->RegisterObjectMethod(MESH_PROP_TYPE_NAME, "void setMesh(MeshResource@)", asMETHOD(MeshProperty, setMeshResource), asCALL_THISCALL);

}