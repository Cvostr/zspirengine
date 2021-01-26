#include "../../../headers/world/ObjectsComponents/CameraComponent.hpp"

Engine::CameraComponent::CameraComponent() {
	type = PROPERTY_TYPE::GO_PROPERTY_TYPE_CAMERA;
	Camera();
	mIsMainCamera = true;
	//this->mTarget = allocTexture();
	//mTarget->Init();
}

void Engine::CameraComponent::onUpdate(float deltaTime) {

}

void Engine::CameraComponent::loadPropertyFromMemory(const char* data, GameObject* obj) {
	unsigned int offset = 1;
	//read collider type
	readBinaryValue(&mProjectionType, data + offset, offset);
	readBinaryValue(&mNearZ, data + offset, offset);
	readBinaryValue(&mFarZ, data + offset, offset);
	readBinaryValue(&mFOV, data + offset, offset);
}
void Engine::CameraComponent::savePropertyToStream(ZsStream* stream, GameObject* obj) {
	stream->writeBinaryValue(&mProjectionType);
    stream->writeBinaryValue(&mNearZ);
    stream->writeBinaryValue(&mFarZ);
    stream->writeBinaryValue(&mFOV);
}

void Engine::CameraComponent::onPreRender(Engine::Renderer* pipeline) {
	pipeline->addCamera(this);

	TransformProperty* transform = go_link.updLinkPtr()->getTransformProperty();

	mCameraPos = transform->abs_translation;
	mCameraFront = _getDirection(transform->abs_rotation);

	updateViewMat();
}

void Engine::CameraComponent::bindObjectPropertyToAngel(AGScriptMgr* mgr) {

}