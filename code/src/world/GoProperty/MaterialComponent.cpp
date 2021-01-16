#include "../../../headers/world/ObjectsComponents/MaterialComponent.hpp"
#include "../../../headers/game.h"

extern ZSGAME_DATA* game_data;

Engine::MaterialProperty::MaterialProperty() :
    mReceiveShadows(true),
    mMaterial(nullptr)
{
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_MATERIAL;
    setMaterial(game_data->resources->getMaterialByLabel("@default"));
}

void Engine::MaterialProperty::copyTo(Engine::IGameObjectComponent* dest) {
    //MaterialShaderProperty
    if (dest->type != PROPERTY_TYPE::GO_PROPERTY_TYPE_MATERIAL) return;

    //Do base things
    IGameObjectComponent::copyTo(dest);

    MaterialProperty* mat_prop = static_cast<MaterialProperty*>(dest);
    mat_prop->material_path = this->material_path;
    mat_prop->mMaterial = this->mMaterial;
    mat_prop->mTemplateLabel = this->mTemplateLabel;
}

void Engine::MaterialProperty::setMaterial(Material* mat) {
    this->mMaterial = mat;
    this->material_path = mat->file_path;
    this->mTemplateLabel = mat->mTemplate->Label;
}

void Engine::MaterialProperty::setMaterial(std::string path) {
    Material* newmat_ptr = game_data->resources->getMaterialByLabel(path)->material;
    setMaterial(newmat_ptr);
}

void Engine::MaterialProperty::setMaterial(MaterialResource* mat) {
    setMaterial(mat->material);
}

void Engine::MaterialProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
    unsigned int offset = 1;
    //Read material path
    readString(material_path, data, offset);
    //get material by label
    MaterialResource* mat_resource = game_data->resources->getMaterialByLabel(material_path);
    if (mat_resource == nullptr)
        return;
    mMaterial = mat_resource->material; //find it and process
    //Read receiveShadows boolean
    readBinaryValue(&mReceiveShadows, data + offset, offset);
    mTemplateLabel = mMaterial->mTemplate->Label;
}

void Engine::MaterialProperty::savePropertyToStream(ZsStream* stream, GameObject* obj) {
    //Write path to material string
    if (mMaterial != nullptr)
        stream->writeString(material_path);//Write material relpath
    else
        stream->writeString("@none");

    stream->writeBinaryValue(&mReceiveShadows);
}

void Engine::MaterialProperty::bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr) {
    mgr->RegisterObjectType(MAT_PROP_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);

    mgr->RegisterObjectMethod(MAT_PROP_TYPE_NAME, "void setMaterial(MaterialResource@)", asMETHODPR(MaterialProperty, setMaterial, (MaterialResource*), void), asCALL_THISCALL);
}