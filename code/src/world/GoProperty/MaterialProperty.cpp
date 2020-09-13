#include "../../../headers/world/go_properties.h"

extern ZSGAME_DATA* game_data;

Engine::MaterialProperty::MaterialProperty() {
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_MATERIAL;

    receiveShadows = true;
    this->material_ptr = nullptr;
}

void Engine::MaterialProperty::copyTo(Engine::GameObjectProperty* dest) {
    //MaterialShaderProperty
    if (dest->type != PROPERTY_TYPE::GO_PROPERTY_TYPE_MATERIAL) return;

    //Do base things
    GameObjectProperty::copyTo(dest);

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

    offset++;
    //Read receiveShadows boolean
    readBinaryValue<bool>(&receiveShadows, data + offset, offset);
    this->group_label = material_ptr->group_ptr->groupCaption;
}

void Engine::MaterialProperty::savePropertyToStream(std::ofstream* stream, GameObject* obj) {
    //Write path to material string
    if (material_ptr != nullptr)
        *stream << material_path << '\0'; //Write material relpath
    else
        *stream << "@none" << '\0';

    stream->write(reinterpret_cast<char*>(&receiveShadows), sizeof(bool));
}

void Engine::MaterialProperty::bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr) {
    mgr->RegisterObjectType(MAT_PROP_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);

    mgr->RegisterObjectMethod(MAT_PROP_TYPE_NAME, "void setMaterial(MaterialResource@)", asMETHOD(MaterialProperty, _setMaterial), asCALL_THISCALL);

}