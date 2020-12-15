#include "../../../headers/world/tile_properties.h"
#include "../../../headers/game.h"

extern ZSGAME_DATA* game_data;

Engine::TileGroupProperty::TileGroupProperty(){
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_TILE_GROUP; //Set correct type
    active = true; //And it is active

    this->isCreated = false;
    this->tiles_amount_X = 0;
    this->tiles_amount_Y = 0;

    this->mesh_string = "@plane";
    this->diffuse_relpath = "@none";
}

void Engine::TileGroupProperty::copyTo(Engine::IGameObjectComponent* dest){
    if(dest->type != this->type) return; //if it isn't transform

    //Do base things
    Engine::IGameObjectComponent::copyTo(dest);

    TileGroupProperty* _dest = static_cast<TileGroupProperty*>(dest);
    //Transfer all variables
    _dest->geometry = geometry;
    _dest->tiles_amount_X = tiles_amount_X;
    _dest->tiles_amount_Y = tiles_amount_Y;
    _dest->isCreated = isCreated;
}

void Engine::TileGroupProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
    unsigned int offset = 1; //Skip space
    int isCreated = 0;

    memcpy(&isCreated, data + offset, sizeof(int));
    offset += sizeof(int);
    //Write size
    memcpy(&geometry.tileWidth, data + offset, sizeof(int));
    offset += sizeof(int);
    memcpy(&geometry.tileHeight, data + offset, sizeof(int));
    offset += sizeof(int);
    //Write amounts
    memcpy(&tiles_amount_X, data + offset, sizeof(int));
    offset += sizeof(int);
    memcpy(&tiles_amount_Y, data + offset, sizeof(int));
    offset += sizeof(int);

    this->isCreated = static_cast<bool>(isCreated);

    //Read diffuse and mesh
    diffuse_relpath.clear();
    while (data[offset] != '\0' && data[offset] != '\n') {
        diffuse_relpath += data[offset];
        offset++;
    }
    offset++;
    mesh_string.clear();
    while (data[offset] != '\0' && data[offset] != '\n') {
        mesh_string += data[offset];
        offset++;
    }
}

void Engine::TileGroupProperty::savePropertyToStream(ZsStream* stream, GameObject* obj) {
    int isCreated = static_cast<int>(this->isCreated);

    stream->write(reinterpret_cast<char*>(&isCreated), sizeof(int));
    stream->write(reinterpret_cast<char*>(&geometry.tileWidth), sizeof(int));
    stream->write(reinterpret_cast<char*>(&geometry.tileHeight), sizeof(int));
    stream->write(reinterpret_cast<char*>(&tiles_amount_X), sizeof(int));
    stream->write(reinterpret_cast<char*>(&tiles_amount_Y), sizeof(int));

    *stream << diffuse_relpath << '\0' << mesh_string << '\0';
}

Engine::TileProperty::TileProperty(){
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_TILE;
    active = true;

    this->texture_diffuse = nullptr;
    this->diffuse_relpath = "@none";

    this->texture_transparent = nullptr;
    this->transparent_relpath = "@none";
}
void Engine::TileProperty::updTexturePtr(){
    //Update color texture
    this->texture_diffuse = game_data->resources->getTextureByLabel(diffuse_relpath);
    //Update transparent layer texture
    this->texture_transparent = game_data->resources->getTextureByLabel(transparent_relpath);
}

void Engine::TileProperty::onUpdate(float deltaTime){
    if(this->anim_state.playing == true){ //if we playing anim
        anim_state.current_time += static_cast<int>(deltaTime);
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

void Engine::TileProperty::copyTo(Engine::IGameObjectComponent* dest){
    if(dest->type != this->type) return; //if it isn't transform

    //Do base things
    Engine::IGameObjectComponent::copyTo(dest);

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

void Engine::TileProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
    unsigned int offset = 0;
    offset += 1; //Skip space
    //read textures relative pathes
    readString(diffuse_relpath, data, offset);

    readString(transparent_relpath, data, offset);
    //set pointers to textures
    updTexturePtr();

    readBinaryValue(&anim_property.isAnimated, data + offset, offset);

    if (anim_property.isAnimated) { //if animated, then write animation properties
        readBinaryValue(&anim_property.framesX, data + offset, offset);
        readBinaryValue(&anim_property.framesY, data + offset, offset);
    }
}

void Engine::TileProperty::savePropertyToStream(ZsStream* stream, GameObject* obj) {
    if (diffuse_relpath.empty()) //check if object has no texture
        *stream << "@none";
    else
        *stream << diffuse_relpath << '\0';

    if (transparent_relpath.empty()) //check if object has no texture
        *stream << "@none";
    else
        *stream << transparent_relpath << '\0';

    //Animation stuff
    stream->writeBinaryValue(&anim_property.isAnimated);
    if (anim_property.isAnimated) { //if animated, then write animation properties
        stream->writeBinaryValue(&anim_property.framesX);
        stream->writeBinaryValue(&anim_property.framesY);
    }
}