#include "../../../headers/world/tile_properties.h"

Engine::TileGroupProperty::TileGroupProperty(){
    type = GO_PROPERTY_TYPE_TILE_GROUP;
    isCreated = false;
}

Engine::TileProperty::TileProperty(){
    type = GO_PROPERTY_TYPE_TILE;
    lastAnimState = false;
}

void Engine::TileProperty::updTexturePtr(){
    this->texture_diffuse = this->go_link.world_ptr->getResourceManager()->getTextureByLabel(this->diffuse_relpath);
    this->texture_transparent = this->go_link.world_ptr->getResourceManager()->getTextureByLabel(this->transparent_relpath);
}
void Engine::TileProperty::onUpdate(float deltaTime){
    if(this->anim_state.playing == true){ //if we playing anim
        anim_state.current_time += deltaTime;
        if(anim_state.current_time >= anim_property.frame_time){ //if its time to switch frame
            anim_state.current_time = 0; //make time zero
            anim_state.current_frame += 1; //switch to next frame

            anim_state.cur_frameX = anim_state.current_frame % anim_property.framesX;
            anim_state.cur_frameY = anim_state.current_frame / anim_property.framesX;
        }
        //if we played all the frames
        if(anim_state.current_frame > anim_property.framesX * anim_property.framesY){
            //start from beginning
            anim_state.current_time = 0;
            anim_state.current_frame = 0;

            anim_state.cur_frameX = 0;
            anim_state.cur_frameY = 0;
        }

    }
}

void Engine::TileProperty::copyTo(GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't transform

    //Do base things
    GameObjectProperty::copyTo(dest);

    TileProperty* _dest = static_cast<TileProperty*>(dest);
    _dest->diffuse_relpath = diffuse_relpath;
    _dest->texture_diffuse = texture_diffuse;
    _dest->anim_property = anim_property;
    _dest->geometry = geometry;
}

void Engine::TileProperty::playAnim(){
    anim_state.playing = true;
}
void Engine::TileProperty::stopAnim(){
    anim_state.playing = false;
}
void Engine::TileProperty::setDiffuseTexture(std::string texture){
    diffuse_relpath = texture;
        //Update texture pointer
    updTexturePtr();
}
