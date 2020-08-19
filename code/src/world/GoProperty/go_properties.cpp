#include "../../../headers/world/go_properties.h"
#include "../../../headers/world/tile_properties.h"

extern ZSGAME_DATA* game_data;

Engine::GameObjectProperty::GameObjectProperty(){
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_NONE;
    active = true;
    world_ptr = nullptr;
}
void Engine::GameObjectProperty::setActive(bool active){
    this->active = active;
}
bool Engine::GameObjectProperty::isActive(){
    return active && go_link.updLinkPtr()->active;
}
void Engine::GameObjectProperty::copyTo(GameObjectProperty* dest){
    dest->active = this->active;
    dest->world_ptr = this->world_ptr;
}
void Engine::GameObjectProperty::onStart() {

}
void Engine::GameObjectProperty::onStop() {

}
void Engine::GameObjectProperty::onUpdate(float deltaTime){

}
void Engine::GameObjectProperty::onPreRender(RenderPipeline* pipeline){

}
void Engine::GameObjectProperty::onRender(RenderPipeline* pipeline){

}
void Engine::GameObjectProperty::onAddToObject(){

}
void Engine::GameObjectProperty::onObjectDeleted(){

}
void Engine::GameObjectProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {

}
void Engine::GameObjectProperty::bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr) {

}
void Engine::GameObjectProperty::onTrigger(Engine::GameObject* obj){
    assert(obj);
}

void Engine::GameObjectProperty::addPropertyInterfaceToInspector(){
}

void Engine::GameObjectProperty::onValueChanged(){

}

Engine::GameObjectProperty::~GameObjectProperty(){

}

Engine::GameObjectProperty* Engine::allocProperty(PROPERTY_TYPE type){
    GameObjectProperty* _ptr = nullptr;
    PROPERTY_TYPE _type = static_cast<PROPERTY_TYPE>(type);
    switch (_type) {
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_TRANSFORM:{ //If type is transfrom
            _ptr = static_cast<GameObjectProperty*>(new TransformProperty); //Allocation of transform in heap
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_LABEL:{
            LabelProperty* ptr = new LabelProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_MESH:{
            _ptr = static_cast<GameObjectProperty*>(new MeshProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_NODE:{
            _ptr = static_cast<GameObjectProperty*>(new NodeProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_LIGHTSOURCE:{
            _ptr = static_cast<GameObjectProperty*>(new LightsourceProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_AGSCRIPT:{
            ZPScriptProperty* ptr = new ZPScriptProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_AUDSOURCE:{
            _ptr = static_cast<GameObjectProperty*>(new AudioSourceProperty);
            break;
        }

        case PROPERTY_TYPE::GO_PROPERTY_TYPE_MATERIAL:{
            _ptr = static_cast<GameObjectProperty*>(new MaterialProperty);
            break;
        }

        case PROPERTY_TYPE::GO_PROPERTY_TYPE_COLLIDER:{
            _ptr = static_cast<GameObjectProperty*>(new ColliderProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_RIGIDBODY:{
            _ptr = static_cast<Engine::GameObjectProperty*>(new Engine::RigidbodyProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_CHARACTER_CONTROLLER:{
            _ptr = static_cast<Engine::GameObjectProperty*>(new Engine::CharacterControllerProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_TILE_GROUP:{
            _ptr = static_cast<GameObjectProperty*>(new TileGroupProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_TILE:{
            _ptr = static_cast<GameObjectProperty*>(new TileProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_ANIMATION:{
            _ptr = static_cast<GameObjectProperty*>(new AnimationProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_SKYBOX:{
            _ptr = static_cast<Engine::GameObjectProperty*>(new Engine::SkyboxProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_SHADOWCASTER:{
            _ptr = static_cast<Engine::GameObjectProperty*>(new Engine::ShadowCasterProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_TERRAIN:{
            _ptr = static_cast<Engine::GameObjectProperty*>(new Engine::TerrainProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_TRIGGER: {
            _ptr = static_cast<Engine::GameObjectProperty*>(new Engine::TriggerProperty);
            break;
        }
    }
    return _ptr;
}


Engine::LabelProperty::LabelProperty(){
    this->type = PROPERTY_TYPE::GO_PROPERTY_TYPE_LABEL;
}

Engine::LabelProperty::~LabelProperty() {

}

void Engine::LabelProperty::copyTo(Engine::GameObjectProperty* dest) {
    if (dest->type != this->type) return; //if it isn't label

    //Do base things
    GameObjectProperty::copyTo(dest);

    LabelProperty* _dest = static_cast<LabelProperty*>(dest);
    _dest->label = label;
}


void Engine::LabelProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
    unsigned int offset = 1;
    std::string label;
    while (data[offset] != ' ' && data[offset] != '\n') {
        label += data[offset];
        offset++;
    }
    obj->label_ptr = &this->label; //Making GameObjects's pointer to string in label property
    this->label = label; //Write loaded string
}

Engine::SkyboxProperty::SkyboxProperty(){
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_SKYBOX;
}
