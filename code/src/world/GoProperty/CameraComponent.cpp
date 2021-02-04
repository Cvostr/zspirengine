#include "../../../headers/world/ObjectsComponents/CameraComponent.hpp"
#include "../../../headers/game.h"

extern ZSGAME_DATA* game_data;

Engine::CameraComponent::CameraComponent() {
	type = PROPERTY_TYPE::GO_PROPERTY_TYPE_CAMERA;
	Camera();
	mIsMainCamera = true;
	mViewMask = 0xFFFFFFFFFFFFFFFF;
}

void Engine::CameraComponent::onUpdate(float deltaTime) {

}

void Engine::CameraComponent::UpdateTextureResource() {
	mTarget = game_data->resources->getTextureByLabel(TargetResourceName);
}

void Engine::CameraComponent::loadPropertyFromMemory(const char* data, GameObject* obj) {
	unsigned int offset = 1;

	readString(TargetResourceName, data, offset);
	//read collider type
	readBinaryValue(&mProjectionType, data + offset, offset);
	readBinaryValue(&mNearZ, data + offset, offset);
	readBinaryValue(&mFarZ, data + offset, offset);
	readBinaryValue(&mFOV, data + offset, offset);
	readBinaryValue(&mIsMainCamera, data + offset, offset);

	UpdateTextureResource();
}

void Engine::CameraComponent::copyTo(Engine::IGameObjectComponent* dest) {
	if (dest->type != this->type) return; //if it isn't audiosource then exit

	CameraComponent* _dest = static_cast<CameraComponent*>(dest);

	//Do base things
	IGameObjectComponent::copyTo(dest);

	_dest->TargetResourceName = this->TargetResourceName;
	_dest->mIsMainCamera = this->mIsMainCamera;

	_dest->mProjectionType = this->mProjectionType;
	_dest->mNearZ = this->mNearZ;
	_dest->mFarZ = this->mFarZ;
	_dest->mFOV = this->mFOV;
}

void Engine::CameraComponent::savePropertyToStream(ZsStream* stream, GameObject* obj) {

	if (TargetResourceName.empty()) //check if object has no audioclip
		stream->writeString("@none");
	else
		stream->writeString(TargetResourceName);

	stream->writeBinaryValue(&mProjectionType);
    stream->writeBinaryValue(&mNearZ);
    stream->writeBinaryValue(&mFarZ);
    stream->writeBinaryValue(&mFOV);
	stream->writeBinaryValue(&mIsMainCamera);
}

void Engine::CameraComponent::onPreRender(Engine::Renderer* pipeline) {
	pipeline->addCamera(this);

	TransformProperty* transform = go_link.updLinkPtr()->getTransformProperty();

	mCameraPos = transform->abs_translation;

	float yaw = transform->abs_rotation.Y;
	float pitch = transform->abs_rotation.X;

	mCameraFront.X = cosf(DegToRad(yaw)) * cosf(DegToRad(pitch));
	mCameraFront.Y = -sinf(DegToRad(pitch));
	mCameraFront.Z = sinf(DegToRad(yaw)) * cosf(DegToRad(pitch));
	mCameraFront.Normalize();

	updateViewMat();
}

void Engine::CameraComponent::bindObjectPropertyToAngel(AGScriptMgr* mgr) {
	mgr->RegisterObjectType(CAMERA_PROP_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);

	mgr->RegisterObjectMethod(CAMERA_PROP_TYPE_NAME, "void SetUp(const Vec3 &in)", asMETHOD(CameraComponent, setUp), asCALL_THISCALL);
	mgr->RegisterObjectMethod(CAMERA_PROP_TYPE_NAME, "void SetViewScale(const Vec3 &in)", asMETHOD(CameraComponent, setViewScale), asCALL_THISCALL);
	mgr->RegisterObjectMethod(CAMERA_PROP_TYPE_NAME, "void SetReflectionPlane(Plane &in)", asMETHOD(CameraComponent, setReflectionPlane), asCALL_THISCALL);

	mgr->RegisterObjectMethod(CAMERA_PROP_TYPE_NAME, "void SetViewMask(uint64)", asMETHOD(CameraComponent, SetViewMask), asCALL_THISCALL);

}