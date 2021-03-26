#include <world/ObjectsComponents/ParticleEmitterComponent.hpp>
#include <game.h>

extern ZSGAME_DATA* game_data;
static long long int ParticlesRandomSeed = 3467828329819;

int Rand()
{
	ParticlesRandomSeed = ParticlesRandomSeed * 214013 + 2531011;
	return (ParticlesRandomSeed >> 16u) & 32767u;
}

void Engine::ParticleEmitterComponent::onUpdate(float deltaTime) {
	
}
void Engine::ParticleEmitterComponent::copyTo(Engine::IGameObjectComponent* dest) {
	if (dest->type != this->type) return; //if it isn't Lightsource, then exit

	//Do base things
	IGameObjectComponent::copyTo(dest);

	ParticleEmitterComponent* _dest = static_cast<ParticleEmitterComponent*>(dest);
	_dest->mMeshResLabel = mMeshResLabel;
	_dest->mShape = mShape;
	_dest->mDuration = mDuration;
	_dest->mLooping = mLooping;
	_dest->mPrewarm = mPrewarm;
	_dest->mLifetime = mLifetime;
	_dest->mMaxParticles = mMaxParticles;

	_dest->mDirection = mDirection;
	_dest->mSize = mSize;
	_dest->mVelocity = mVelocity;
	_dest->mConstantForce = mConstantForce;
	_dest->mDampingForce = mDampingForce;
	_dest->mRotation = mRotation;
	_dest->mRotationSpeed = mRotationSpeed;

}

void Engine::ParticleEmitterComponent::onStart() {

}

void Engine::ParticleEmitterComponent::loadPropertyFromMemory(const char* data, GameObject* obj) {
	unsigned int offset = 1;

	readBinaryValue(&mShape, data + offset, offset);
	readBinaryValue(&mDuration, data + offset, offset);
	readBinaryValue(&mLooping, data + offset, offset);
	readBinaryValue(&mPrewarm, data + offset, offset);
	readBinaryValue(&mLifetime, data + offset, offset);
	readBinaryValue(&mMaxParticles, data + offset, offset);

	readBinaryValue(&mEmissionRate.Min, data + offset, offset);
	readBinaryValue(&mEmissionRate.Max, data + offset, offset);

	readBinaryValue(&mDirection.Min, data + offset, offset);
	readBinaryValue(&mDirection.Max, data + offset, offset);

	readBinaryValue(&mSize.OriginalValue.Min, data + offset, offset);
	readBinaryValue(&mSize.OriginalValue.Max, data + offset, offset);
	readBinaryValue(&mSize.Add, data + offset, offset);
	readBinaryValue(&mSize.Mul, data + offset, offset);

	readBinaryValue(&mVelocity.Min, data + offset, offset);
	readBinaryValue(&mVelocity.Max, data + offset, offset);

	readBinaryValue(&mConstantForce, data + offset, offset);
	readBinaryValue(&mDampingForce, data + offset, offset);

	readBinaryValue(&mRotation.Min, data + offset, offset);
	readBinaryValue(&mRotation.Max, data + offset, offset);
	readBinaryValue(&mRotationSpeed.Min, data + offset, offset);
	readBinaryValue(&mRotationSpeed.Max, data + offset, offset);

	readString(mMeshResLabel, data, offset);
	readString(mMaterialResLabel, data, offset);
	
	updateMeshPtr();
}
void Engine::ParticleEmitterComponent::savePropertyToStream(ZsStream* stream, GameObject* obj) {
	stream->writeBinaryValue(&mShape);
	stream->writeBinaryValue(&mDuration);
	stream->writeBinaryValue(&mLooping);
	stream->writeBinaryValue(&mPrewarm);
	stream->writeBinaryValue(&mLifetime);
	stream->writeBinaryValue(&mMaxParticles);

	stream->writeBinaryValue(&mEmissionRate.Min);
	stream->writeBinaryValue(&mEmissionRate.Max);

	stream->writeVec3(mDirection.Min);
	stream->writeVec3(mDirection.Max);
	
	stream->writeVec2(mSize.OriginalValue.Min);
	stream->writeVec2(mSize.OriginalValue.Max);
	stream->writeBinaryValue(&mSize.Add);
	stream->writeBinaryValue(&mSize.Mul);

	stream->writeBinaryValue(&mVelocity.Min);
	stream->writeBinaryValue(&mVelocity.Max);

	stream->writeVec3(mConstantForce);
	stream->writeBinaryValue(&mDampingForce);

	stream->writeBinaryValue(&mRotation.Min);
	stream->writeBinaryValue(&mRotation.Max);
	stream->writeBinaryValue(&mRotationSpeed.Min);
	stream->writeBinaryValue(&mRotationSpeed.Max);

	stream->writeString(mMeshResLabel);
	stream->writeString(mMaterialResLabel);
}

void Engine::ParticleEmitterComponent::StartSimulation() {
	mParticles.resize(mMaxParticles);
	TransformProperty* transform = go_link.updLinkPtr()->getTransformProperty();

	for (unsigned int particle_i = 0; particle_i < mMaxParticles; particle_i++) {
		mParticles[particle_i] = new Particle;
	}

	mSimulating = true;
	mSimulationTime = 0;
	mEmitterTime = 0;
}

bool Engine::ParticleEmitterComponent::EmitNewParticle() {
	TransformProperty* transform = go_link.updLinkPtr()->getTransformProperty();

	uint32_t FreeIndex = GetFreeParticleIndex();

	if (FreeIndex == 0xFFFFFFFF || FreeIndex > (mMaxParticles - 1))
		return false;

	Particle* particlePtr = mParticles[FreeIndex];

	Vec3 BaseDir = _getDirection(transform->abs_rotation);

	particlePtr->Size = GetRandomSize();
	particlePtr->Color = this->Color.OriginalValue;

	Vec3 Dir = BaseDir + GetRandomDirection();
	Dir.Normalize();

	switch (mShape) {
	case PE_SHAPE_SPHERE: {

		Vec3 vel = Vec3(
			GetRandomFloat(2.f) - 1.0f,
			GetRandomFloat(2.f) - 1.0f,
			GetRandomFloat(2.f) - 1.0f
		);
		vel.Normalize();

		particlePtr->Position = transform->abs_translation + transform->abs_scale * vel * 0.5f;

		break;
	}
	case PE_SHAPE_BOX: {
		Vec3 Size = transform->abs_scale;
		particlePtr->Position = Vec3(
			GetRandomFloat(Size.X) - Size.X * 0.5f,
			GetRandomFloat(Size.Y) - Size.Y * 0.5f,
			GetRandomFloat(Size.Z) - Size.Z * 0.5f
		);

		break;
	}
	}

	particlePtr->Rotation = GetRandomRotation();
	particlePtr->RotationSpeed = GetRandomRotationSpeed();
	particlePtr->Velocity = Dir * GetRandomVelocity();
	particlePtr->mAlive = true;
	particlePtr->mTimePassed = 0;
	//particlePtr->Position += Dir * particlePtr->Size.Y;

	return true;
}

void Engine::ParticleEmitterComponent::GetNewParticleVelocityPos(Vec3& Velocity, Vec3& Pos) {
	
}

void Engine::ParticleEmitterComponent::DestroyParticle(Particle* Particle) {
	Particle->mAlive = false;
}

void Engine::ParticleEmitterComponent::StopSimulation() {
	for (int i = 0; i < mParticles.size(); i++) {
		delete mParticles[i];
	}

	mParticles.clear();

	mSimulating = false;
}

void Engine::ParticleEmitterComponent::StepSimulation() {
	if (!mSimulating)
		return;

	if (mSimulationTime >= mDuration) {
		if (mLooping) {
			if(!mPrewarm)
				RestartSimulation();
		}
		else {
			StopSimulation();
		}
	}

	float DeltaTime = game_data->time->GetDeltaTime();

	mSimulationTime += DeltaTime;
	mEmitterTime += DeltaTime;

	int32_t leftToEmit = mMaxParticles - GetAliveParticlesCount();

	while (leftToEmit > 0 && mEmitterTime > 0) {
		if (EmitNewParticle()) {
			mEmitterTime -= (1.f / GetRandomEmissionRate());
			leftToEmit--;
		}
		else
			break;
	}

	for (uint32_t particle_i = 0; particle_i < GetAliveParticlesCount(); particle_i++) {
		Particle* particlePtr = mParticles[particle_i];

		if (!particlePtr->mAlive)
			continue;

		if (particlePtr->mTimePassed >= mLifetime) {
			DestroyParticle(particlePtr);
			continue;
		}

		//update velocity
		particlePtr->Velocity += mConstantForce * DeltaTime;

		if (mDampingForce != 0.f) {
			Vec3 force = mDampingForce * particlePtr->Velocity;
			particlePtr->Velocity += DeltaTime * force;
		}

		//Update size
		particlePtr->Size = particlePtr->Size + Vec2(mSize.Add * DeltaTime);
		particlePtr->Size *= (mSize.Mul - 1.f) * DeltaTime + 1.f;
		//Update position
		particlePtr->Position += particlePtr->Velocity * DeltaTime;
		//Update rotation
		particlePtr->Rotation += particlePtr->RotationSpeed * DeltaTime;
		//Update particle age
		particlePtr->mTimePassed += DeltaTime;
	}
}

void Engine::ParticleEmitterComponent::RestartSimulation() {
	StopSimulation();
	StartSimulation();
}

void Engine::ParticleEmitterComponent::GetParticlesTransforms(Mat4** Transforms, Camera& cam) {
	uint32_t aliveParticlesCount = GetAliveParticlesCount();
	if (aliveParticlesCount == 0)
		return;
	*Transforms = new Mat4[aliveParticlesCount];
	uint32_t TransformI = 0;

	for (int i = 0; i < mParticles.size(); i++) {
		Particle* Particle = mParticles[i];
		if (Particle->mAlive) {
			Mat4 transformMat = getTranslationMat(Particle->Position);
			transformMat = removeRotationFromTransformMat(transformMat, cam.getViewMatrix());
			transformMat = getScaleMat(Particle->Size.X, Particle->Size.Y, 1) * getRotationZMat(Particle->Rotation) * transformMat;

			Vec3 Rt = Particle->Velocity;
			Rt.Normalize();
			float q1 = Rt.X;
			float q2 = Rt.Y;
			float q3 = Rt.Z;

			/*Mat4 transformMat = getScaleMat(Particle->Size.X, Particle->Size.Y, 1)
				* getRotationMat(Quaternion(q1, q2, q3, 0)) * getRotationZMat(Particle->Rotation)
				* getTranslationMat(Particle->Position);*/
				
			(*Transforms)[TransformI++] = transformMat;
		}
	}

	Vec3 CamPos = cam.getCameraPosition();
	//Sort Array
	for (unsigned int i = 1; i < aliveParticlesCount - 1; i++) {
		for (unsigned int j = 0; j < aliveParticlesCount - i - 1; j++) {
			Vec3 Pos1 = (*Transforms)[j].GetPosition();
			Vec3 Pos2 = (*(*Transforms + (j + 1))).GetPosition();

			float Dist1 = getDistance(CamPos, Pos1);
			float Dist2 = getDistance(CamPos, Pos2);

			if (Dist1 < Dist2) {
				Mat4 temp = (*Transforms)[j];
				(*Transforms)[j] = (*Transforms)[j + 1];
				*(*Transforms + (j + 1)) = temp;
			}
		}
	}

}

Vec3 Engine::ParticleEmitterComponent::GetRandomDirection() {
	return Vec3(
		lerp(mDirection.Min.X, mDirection.Max.X, GetRandomFloat(1.f)),
		lerp(mDirection.Min.Y, mDirection.Max.Y, GetRandomFloat(1.f)),
		lerp(mDirection.Min.Z, mDirection.Max.Z, GetRandomFloat(1.f)));
}

Vec2 Engine::ParticleEmitterComponent::GetRandomSize() {

	Vec2 SizeMin = mSize.OriginalValue.Min;
	Vec2 SizeMax = mSize.OriginalValue.Max;

	return Vec2(
		lerp(SizeMin.X, SizeMax.X, GetRandomFloat(1.f)),
		lerp(SizeMin.Y, SizeMax.Y, GetRandomFloat(1.f)));
}

float Engine::ParticleEmitterComponent::GetRandomVelocity() {
	return lerp(mVelocity.Min, mVelocity.Max, GetRandomFloat(1.f));
}

float Engine::ParticleEmitterComponent::GetRandomRotation() {
	return lerp(mRotation.Min, mRotation.Max, GetRandomFloat(1.f));
}

float Engine::ParticleEmitterComponent::GetRandomRotationSpeed() {
	return lerp(mRotationSpeed.Min, mRotationSpeed.Max, GetRandomFloat(1.f));
}

int Engine::ParticleEmitterComponent::GetRandomEmissionRate() {
	return lerp(mEmissionRate.Min, mEmissionRate.Max, GetRandomFloat(1.f));
}

float Engine::ParticleEmitterComponent::GetRandomFloat(float max) {
	return (Rand() * max) / 32767.f;
}

void Engine::ParticleEmitterComponent::updateMeshPtr() {
	this->mParticleMesh = game_data->resources->getMeshByLabel(this->mMeshResLabel);
	this->mParticleMaterial = game_data->resources->getMaterialByLabel(this->mMaterialResLabel);
}

uint32_t Engine::ParticleEmitterComponent::GetFreeParticleIndex() {
	for (uint32_t i = 0; i < static_cast<uint32_t>(mParticles.size()); i++) {
		if (!mParticles[i]->mAlive)
			return i;
	}
	return 0xFFFFFFFF;
}

uint32_t Engine::ParticleEmitterComponent::GetAliveParticlesCount() {
	uint32_t Result = 0;
	for (uint32_t i = 0; i < static_cast<uint32_t>(mParticles.size()); i++) {
		if (mParticles[i]->mAlive)
			Result += 1;
	}
	return Result;
}

void Engine::ParticleEmitterComponent::bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr) {
	mgr->RegisterObjectType(PARTICLE_EMITTER_PROP_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);


	mgr->RegisterObjectMethod(PARTICLE_EMITTER_PROP_TYPE_NAME, "void Start()", asMETHOD(ParticleEmitterComponent, StartSimulation), asCALL_THISCALL);
	mgr->RegisterObjectMethod(PARTICLE_EMITTER_PROP_TYPE_NAME, "void Stop()", asMETHOD(ParticleEmitterComponent, StopSimulation), asCALL_THISCALL);
	mgr->RegisterObjectMethod(PARTICLE_EMITTER_PROP_TYPE_NAME, "bool IsSimulating()", asMETHOD(ParticleEmitterComponent, IsSimulating), asCALL_THISCALL);

}


Engine::ParticleEmitterComponent::ParticleEmitterComponent() :
	mSimulating(false),
	mShape(PE_SHAPE_SPHERE),
	mDuration(5),
	mLooping(true),
	mPrewarm(true),
	mLifetime(2),
	mMaxParticles(100),
	mDirection(Vec3(0.f, 1.f, 0.f)),
	mVelocity(8.f, 10.f),
	mConstantForce(0, -9.8f, 0),
	mDampingForce(0),
	mRotation(0, 0),
	mRotationSpeed(0, 0),
	mMeshResLabel("@plane"),
	mEmissionRate(200, 200)
{
	type = PROPERTY_TYPE::GO_PROPERTY_TYPE_PARTICLE_EMITTER;
	updateMeshPtr();
}