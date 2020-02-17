#include "../../../headers/world/go_properties.h"

Engine::TerrainProperty::TerrainProperty(){
    type = GO_PROPERTY_TYPE_TERRAIN;

    this->Width = 500;
    this->Length = 500;
    this->MaxHeight = 500;
    GrassDensity = 1.0f;
    castShadows = true;
    textures_size = 0;
    grassType_size = 0;

    this->range = 15;
    this->editHeight = 10;
    this->textureid = 1;
    this->vegetableid = 1;

    edit_mode = 1;

    rigidBody = nullptr;
}

TerrainData* Engine::TerrainProperty::getTerrainData(){
    return &data;
}

void Engine::TerrainProperty::copyTo(Engine::GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't script group

    //Do base things
    GameObjectProperty::copyTo(dest);

    Engine::TerrainProperty* _dest = static_cast<Engine::TerrainProperty*>(dest);
    _dest->Width = this->Width;
    _dest->Length = this->Length;
    _dest->MaxHeight = this->MaxHeight;
    _dest->GrassDensity = this->GrassDensity;
    _dest->file_label = this->file_label;
    _dest->castShadows = this->castShadows;
    _dest->textures_size = this->textures_size;
    _dest->grassType_size = this->grassType_size;
    //Copying terrain data
    data.copyTo(&_dest->data);
    //Copy textures data
    for(unsigned int t_i = 0; t_i < this->textures.size(); t_i ++)
        _dest->textures.push_back(textures[t_i]);

    for(unsigned int g_i = 0; g_i < this->grass.size(); g_i ++)
        _dest->grass.push_back(grass[g_i]);
}
