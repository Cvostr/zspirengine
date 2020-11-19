#include "../../../headers/world/go_properties.h"

extern ZSGAME_DATA* game_data;

Engine::MaterialProperty::MaterialProperty() :
    receiveShadows(true),
    material_ptr(nullptr)
{
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_MATERIAL;
}

void Engine::MaterialProperty::copyTo(Engine::IGameObjectComponent* dest) {
    //MaterialShaderProperty
    if (dest->type != PROPERTY_TYPE::GO_PROPERTY_TYPE_MATERIAL) return;

    //Do base things
    IGameObjectComponent::copyTo(dest);

    MaterialProperty* mat_prop = static_cast<MaterialProperty*>(dest);
    mat_prop->material_path = this->material_path;
    mat_prop->material_ptr = this->material_ptr;
    mat_prop->group_label = this->group_label;
}

void Engine::MaterialProperty::setMaterial(Material* mat) {
    this->material_ptr = mat;
    this->material_path = mat->file_path;
    this->group_label = mat->group_ptr->groupCaption;
}

void Engine::MaterialProperty::setMaterial(std::string path) {
    Material* newmat_ptr = game_data->resources->getMaterialByLabel(path)->material;
    setMaterial(newmat_ptr);
}

void Engine::MaterialProperty::_setMaterial(MaterialResource* mat) {
    setMaterial(mat->material);
}

void Engine::MaterialProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
    unsigned int offset = 1;
    //Read material path
    readString(material_path, data, offset);
    //get material by label
    material_ptr = game_data->resources->getMaterialByLabel(material_path)->material; //find it and process
    //Read receiveShadows boolean
    readBinaryValue<bool>(&receiveShadows, data + offset, offset);
    this->group_label = material_ptr->group_ptr->groupCaption;
}

void Engine::MaterialProperty::savePropertyToStream(ZsStream* stream, GameObject* obj) {
    //Write path to material string
    if (material_ptr != nullptr)
        stream->writeString(material_path);//Write material relpath
    else
        stream->writeString("@none");

    stream->writeBinaryValue(&receiveShadows);
}

void Engine::MaterialProperty::bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr) {
    mgr->RegisterObjectType(MAT_PROP_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);

    mgr->RegisterObjectMethod(MAT_PROP_TYPE_NAME, "void setMaterial(MaterialResource@)", asMETHOD(MaterialProperty, _setMaterial), asCALL_THISCALL);

}