#include "../../../headers/world/ObjectsComponents/ParticleEmitterComponent.hpp"

void Engine::ParticleEmitterComponent::addPropertyInterfaceToInspector() {

}
void Engine::ParticleEmitterComponent::onUpdate(float deltaTime) {

}
void Engine::ParticleEmitterComponent::copyTo(Engine::IGameObjectComponent* dest) {

}
void Engine::ParticleEmitterComponent::onObjectDeleted() {
}
void Engine::ParticleEmitterComponent::onStart() {

}

void Engine::ParticleEmitterComponent::loadPropertyFromMemory(const char* data, GameObject* obj) {

}
void Engine::ParticleEmitterComponent::savePropertyToStream(ZsStream* stream, GameObject* obj) {

}

Engine::ParticleEmitterComponent::ParticleEmitterComponent() {
	type = PROPERTY_TYPE::GO_PROPERTY_TYPE_PARTICLE_EMITTER;
}