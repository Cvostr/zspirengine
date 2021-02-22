#include <world/ObjectsComponents/ParticleEmitterComponent.hpp>

void Engine::ParticleEmitterComponent::onUpdate(float deltaTime) {

}
void Engine::ParticleEmitterComponent::copyTo(Engine::IGameObjectComponent* dest) {
	if (dest->type != this->type) return; //if it isn't Lightsource, then exit

	//Do base things
	IGameObjectComponent::copyTo(dest);

	ParticleEmitterComponent* _dest = static_cast<ParticleEmitterComponent*>(dest);
	_dest->mDuration = mDuration;
	_dest->mLooping = mLooping;
	_dest->mPrewarm = mPrewarm;
	_dest->mLifetime = mLifetime;
	_dest->mMaxParticles = mMaxParticles;

}
void Engine::ParticleEmitterComponent::onObjectDeleted() {
}
void Engine::ParticleEmitterComponent::onStart() {

}

void Engine::ParticleEmitterComponent::loadPropertyFromMemory(const char* data, GameObject* obj) {

}
void Engine::ParticleEmitterComponent::savePropertyToStream(ZsStream* stream, GameObject* obj) {

}

Engine::ParticleEmitterComponent::ParticleEmitterComponent() :
	mDuration(5),
	mLooping(true),
	mPrewarm(true),
	mLifetime(2),
	mMaxParticles(100),
	mSpeed(5),
	mSize(10.f),
	mStartRotation(0)
{
	type = PROPERTY_TYPE::GO_PROPERTY_TYPE_PARTICLE_EMITTER;
}