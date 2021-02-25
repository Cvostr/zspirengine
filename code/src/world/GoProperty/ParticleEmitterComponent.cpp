#include <world/ObjectsComponents/ParticleEmitterComponent.hpp>
#include <game.h>

extern ZSGAME_DATA* game_data;

void Engine::ParticleEmitterComponent::onUpdate(float deltaTime) {

}
void Engine::ParticleEmitterComponent::copyTo(Engine::IGameObjectComponent* dest) {
	if (dest->type != this->type) return; //if it isn't Lightsource, then exit

	//Do base things
	IGameObjectComponent::copyTo(dest);

	ParticleEmitterComponent* _dest = static_cast<ParticleEmitterComponent*>(dest);
	_dest->mShape = mShape;
	_dest->mDuration = mDuration;
	_dest->mLooping = mLooping;
	_dest->mPrewarm = mPrewarm;
	_dest->mLifetime = mLifetime;
	_dest->mMaxParticles = mMaxParticles;

}

void Engine::ParticleEmitterComponent::onStart() {

}

void Engine::ParticleEmitterComponent::loadPropertyFromMemory(const char* data, GameObject* obj) {
	unsigned int offset = 1;

	//readString(TargetResourceName, data, offset);
	//read collider type
	readBinaryValue(&mShape, data + offset, offset);
	readBinaryValue(&mDuration, data + offset, offset);
	readBinaryValue(&mLooping, data + offset, offset);
	readBinaryValue(&mLifetime, data + offset, offset);
	readBinaryValue(&mMaxParticles, data + offset, offset);
	
}
void Engine::ParticleEmitterComponent::savePropertyToStream(ZsStream* stream, GameObject* obj) {
	stream->writeBinaryValue(&mShape);
	stream->writeBinaryValue(&mDuration);
	stream->writeBinaryValue(&mLooping);
	stream->writeBinaryValue(&mLifetime);
	stream->writeBinaryValue(&mMaxParticles);
}

void Engine::ParticleEmitterComponent::StartSimulation() {
	mParticles.resize(mMaxParticles);

	TransformProperty* transform = go_link.updLinkPtr()->getTransformProperty();

	Vec3 BaseVelocity = _getDirection(transform->abs_rotation);
	BaseVelocity.Normalize();
	this->mVelocity.OriginalValue = BaseVelocity;

	for (unsigned int particle_i = 0; particle_i < mMaxParticles; particle_i++) {
		Particle* particlePtr = &mParticles[particle_i];

		particlePtr->Size = this->mSize.OriginalValue;
		particlePtr->Velocity = this->mVelocity.OriginalValue + Vec3(GetRandomValue(1.f, 100.f) / 100);
		particlePtr->Position = transform->abs_translation;
		particlePtr->Color = this->Color.OriginalValue;
	}

	mSimulating = true;
}

void Engine::ParticleEmitterComponent::EmitNewParticle() {

}

void Engine::ParticleEmitterComponent::DestroyParticle(Particle* Particle) {
	
}

void Engine::ParticleEmitterComponent::StopSimulation() {
	mParticles.clear();

	mSimulating = false;
}

void Engine::ParticleEmitterComponent::StepSimulation() {
	if (!mSimulating)
		return;

	float DeltaTime = game_data->time->GetDeltaTime();

	for (unsigned int particle_i = 0; particle_i < mMaxParticles; particle_i++) {
		Particle* particlePtr = &mParticles[particle_i];

		if (particlePtr->mTimePassed >= mLifetime)
			DestroyParticle(particlePtr);

		particlePtr->Velocity += Vec3(mVelocity.Add * DeltaTime);
		particlePtr->Velocity *= mVelocity.Mul * DeltaTime;


		particlePtr->Position += mVelocity.OriginalValue * DeltaTime;

		particlePtr->mTimePassed += DeltaTime;
	}
}

Engine::ParticleEmitterComponent::ParticleEmitterComponent() :
	mSimulating(false),
	mShape(PE_SHAPE_SPHERE),
	mDuration(5),
	mLooping(true),
	mPrewarm(true),
	mLifetime(2),
	mMaxParticles(100),
	mVelocity(Vec3(0.f, 1.f, 0.f)),
	mSize(10.f),
	mStartRotation(0)
{
	type = PROPERTY_TYPE::GO_PROPERTY_TYPE_PARTICLE_EMITTER;
}