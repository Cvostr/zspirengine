#include "../../../headers/world/tile_properties.h"

#include <world/ObjectsComponents/AudioSourceComponent.hpp>
#include <world/ObjectsComponents/AudioListenerComponent.hpp>
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
#include "../../../headers/world/ObjectsComponents/WindZoneComponent.hpp"
#include "../../../headers/world/ObjectsComponents/ParticleEmitterComponent.hpp"
#include "../../../headers/world/ObjectsComponents/CameraComponent.hpp"

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
            _ptr = static_cast<IGameObjectComponent*>(new LightsourceComponent);
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
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_WINDZONE: {
            _ptr = static_cast<IGameObjectComponent*>(new Engine::WindZoneComponent);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_PARTICLE_EMITTER: {
            _ptr = static_cast<IGameObjectComponent*>(new Engine::ParticleEmitterComponent);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_CAMERA: {
            _ptr = static_cast<IGameObjectComponent*>(new Engine::CameraComponent);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_AUDIO_LISTENER: {
            _ptr = static_cast<IGameObjectComponent*>(new Engine::AudioListenerComponent);
            break;
        }
    }
    return _ptr;
}

Engine::SkyboxProperty::SkyboxProperty(){
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_SKYBOX;
}
