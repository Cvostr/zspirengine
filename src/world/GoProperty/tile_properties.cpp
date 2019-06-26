#include "../../../headers/world/tile_properties.h"

Engine::TileGroupProperty::TileGroupProperty(){
    type = GO_PROPERTY_TYPE_TILE_GROUP;
}

Engine::TileProperty::TileProperty(){
    type = GO_PROPERTY_TYPE_TILE;
}

void Engine::TileProperty::updTexturePtr(){
    this->texture_diffuse = this->go_link.world_ptr->getResourceManager()->getTextureByLabel(this->diffuse_relpath);
    this->texture_transparent = this->go_link.world_ptr->getResourceManager()->getTextureByLabel(this->transparent_relpath);
}
void Engine::TileProperty::onUpdate(float deltaTime){

}
