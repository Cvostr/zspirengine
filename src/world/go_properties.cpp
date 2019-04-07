#include "../../headers/world/go_properties.h"
#include "../../headers/world/tile_properties.h"

Engine::GameObjectProperty::GameObjectProperty(){
    type = GO_PROPERTY_TYPE_NONE;
}

void Engine::GameObjectProperty::copyTo(GameObjectProperty* dest){

}




Engine::GameObjectProperty* Engine::GameObject::allocProperty(int type){
    GameObjectProperty* _ptr = nullptr;
    switch (type) {
        case GO_PROPERTY_TYPE_TRANSFORM:{ //If type is transfrom
            _ptr = static_cast<GameObjectProperty*>(new TransformProperty); //Allocation of transform in heap
            break;
        }
        case GO_PROPERTY_TYPE_LABEL:{
            LabelProperty* ptr = new LabelProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
        case GO_PROPERTY_TYPE_MESH:{
            MeshProperty* ptr = new MeshProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
        /*
        case GO_PROPERTY_TYPE_LIGHTSOURCE:{
            LightsourceProperty* ptr = new LightsourceProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
        case GO_PROPERTY_TYPE_SCRIPTGROUP:{
            ScriptGroupProperty* ptr = new ScriptGroupProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
        case GO_PROPERTY_TYPE_AUDSOURCE:{
            AudioSourceProperty* ptr = new AudioSourceProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
        case GO_PROPERTY_TYPE_MATERIAL:{
            MaterialProperty* ptr = new MaterialProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
        case GO_PROPERTY_TYPE_COLLIDER:{
            ColliderProperty* ptr = new ColliderProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
        */
        case GO_PROPERTY_TYPE_TILE_GROUP:{
            TileGroupProperty* ptr = new TileGroupProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
        case GO_PROPERTY_TYPE_TILE:{
            TileProperty* ptr = new TileProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
    }
    return _ptr;
}

Engine::TransformProperty::TransformProperty(){
    this->type = GO_PROPERTY_TYPE_TRANSFORM;
}


Engine::LabelProperty::LabelProperty(){
    this->type = GO_PROPERTY_TYPE_LABEL;
}


Engine::MeshProperty::MeshProperty(){
    this->type = GO_PROPERTY_TYPE_MESH;
}

void Engine::GameObject::loadProperty(std::ifstream* world_stream){
    PROPERTY_TYPE type;
    world_stream->seekg(1, std::ofstream::cur); //Skip space
    world_stream->read(reinterpret_cast<char*>(&type), sizeof(int));
    //Spawn new property with readed type
    addProperty(type);
    GameObjectProperty* prop_ptr = getPropertyPtrByType(type); //get created property
    //since more than 1 properties same type can't be on one gameobject
    switch(type){
        case GO_PROPERTY_TYPE_LABEL :{
            std::string label;
            *world_stream >> label;
            LabelProperty* lptr = static_cast<LabelProperty*>(prop_ptr);
            this->label_ptr = &lptr->label; //Making GameObjects's pointer to string in label property
            lptr->label = label; //Write loaded string
            break;
        }
    }
}
