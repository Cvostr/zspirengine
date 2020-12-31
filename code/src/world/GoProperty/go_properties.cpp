#include "../../../headers/world/tile_properties.h"

#include "../../../headers/world/ObjectsComponents/AudioSourceComponent.hpp"
#include "../../../headers/world/ObjectsComponents/ZPScriptComponent.hpp"
#include "../../../headers/world/ObjectsComponents/LightSourceComponent.hpp"
#include "../../../headers/world/ObjectsComponents/ShadowCasterComponent.hpp"
#include "../../../headers/world/ObjectsComponents/SkyboxComponent.hpp"
#include "../../../headers/world/ObjectsComponents/TerrainComponent.hpp"
#include "../../../headers/world/ObjectsComponents/MaterialComponent.hpp"
#include "../../../headers/world/ObjectsComponents/MeshComponent.hpp"
#include "../../../headers/world/ObjectsComponents/AnimationComponent.hpp"
#include "../../../headers/world/ObjectsComponents/NodeComponent.hpp"
#include "../../../headers/world/ObjectsComponents/ColliderComponent.hpp"
#include "../../../headers/world/ObjectsComponents/CharacterController.hpp"
#include "../../../headers/world/ObjectsComponents/RigidbodyComponent.hpp"
#include "../../../headers/world/ObjectsComponents/TriggerComponent.hpp"

extern ZSGAME_DATA* game_data;

Engine::IGameObjectComponent::IGameObjectComponent() :
    type(PROPERTY_TYPE::GO_PROPERTY_TYPE_NONE),
    active(true),
    mWorld(nullptr)
{
}
void Engine::IGameObjectComponent::setActive(bool active){
    this->active = active;
}
bool Engine::IGameObjectComponent::isActive(){
    return active && go_link.updLinkPtr()->mActive;
}
void Engine::IGameObjectComponent::copyTo(IGameObjectComponent* dest){
    dest->active = this->active;
    dest->mWorld = this->mWorld;
}
void Engine::IGameObjectComponent::loadPropertyFromMemory(const char* data, GameObject* obj) {

}
void Engine::IGameObjectComponent::savePropertyToStream(ZsStream* stream, GameObject* obj) {

}
void Engine::IGameObjectComponent::bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr) {

}
void Engine::IGameObjectComponent::onTrigger(Engine::GameObject* obj){
    assert(obj);
}

void Engine::IGameObjectComponent::onTriggerEnter(Engine::GameObject* obj) {

}

void Engine::IGameObjectComponent::onTriggerExit(Engine::GameObject* obj) {

}
Engine::IGameObjectComponent::~IGameObjectComponent(){

}

Engine::IGameObjectComponent* Engine::allocProperty(PROPERTY_TYPE type){
    IGameObjectComponent* _ptr = nullptr;
    PROPERTY_TYPE _type = static_cast<PROPERTY_TYPE>(type);
    switch (_type) {
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_TRANSFORM:{ //If type is transfrom
            _ptr = static_cast<IGameObjectComponent*>(new TransformProperty); //Allocation of transform in heap
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_LABEL:{
            LabelProperty* ptr = new LabelProperty;
            _ptr = static_cast<IGameObjectComponent*>(ptr);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_MESH:{
            _ptr = static_cast<IGameObjectComponent*>(new MeshProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_NODE:{
            _ptr = static_cast<IGameObjectComponent*>(new NodeProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_LIGHTSOURCE:{
            _ptr = static_cast<IGameObjectComponent*>(new LightsourceProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_AGSCRIPT:{
            ZPScriptComponent* ptr = new ZPScriptComponent;
            _ptr = static_cast<IGameObjectComponent*>(ptr);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_AUDSOURCE:{
            _ptr = static_cast<IGameObjectComponent*>(new AudioSourceProperty);
            break;
        }

        case PROPERTY_TYPE::GO_PROPERTY_TYPE_MATERIAL:{
            _ptr = static_cast<IGameObjectComponent*>(new MaterialProperty);
            break;
        }

        case PROPERTY_TYPE::GO_PROPERTY_TYPE_COLLIDER:{
            _ptr = static_cast<IGameObjectComponent*>(new ColliderProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_RIGIDBODY:{
            _ptr = static_cast<IGameObjectComponent*>(new Engine::RigidbodyProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_CHARACTER_CONTROLLER:{
            _ptr = static_cast<IGameObjectComponent*>(new Engine::CharacterControllerProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_TILE_GROUP:{
            _ptr = static_cast<IGameObjectComponent*>(new TileGroupProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_TILE:{
            _ptr = static_cast<IGameObjectComponent*>(new TileProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_ANIMATION:{
            _ptr = static_cast<IGameObjectComponent*>(new AnimationProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_SKYBOX:{
            _ptr = static_cast<IGameObjectComponent*>(new Engine::SkyboxProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_SHADOWCASTER:{
            _ptr = static_cast<IGameObjectComponent*>(new Engine::ShadowCasterProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_TERRAIN:{
            _ptr = static_cast<IGameObjectComponent*>(new Engine::TerrainProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_TRIGGER: {
            _ptr = static_cast<IGameObjectComponent*>(new Engine::TriggerProperty);
            break;
        }
    }
    return _ptr;
}

Engine::SkyboxProperty::SkyboxProperty(){
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_SKYBOX;
}
