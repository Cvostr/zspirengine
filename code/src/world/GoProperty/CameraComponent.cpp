#include <world/ObjectsComponents/CameraComponent.hpp>
#include <game.h>

extern ZSGAME_DATA* game_data;

Engine::CameraComponent::CameraComponent() {
	type = PROPERTY_TYPE::GO_PROPERTY_TYPE_CAMERA;
	Camera();
	mIsMainCamera = true;
	mViewMask = 0xFFFFFFFFFFFFFFFF;
	mCullFaceDirection = CCF_DIRECTION_CCW;

	mGBuffer = nullptr;
	mDefferedBuffer = nullptr;

	TargetWidth = 640;
	TargetHeight = 480;
}

void Engine::CameraComponent::onUpdate(float deltaTime) {

}

void Engine::CameraComponent::UpdateTextureResource() {
	TextureResource* TargetResource = game_data->resources->getTextureByLabel(TargetResourceName);
	//Check, if target texture resource is changed
	if (mTarget != TargetResource) {
		TargetResource->loadAndWait();
		//Target is changed
		mTarget = TargetResource;
		//Get sizes of new target
		TargetWidth = mTarget->texture_ptr->GetWidth();
		TargetHeight = mTarget->texture_ptr->GetHeight();
		//If deffered buffer is already created - delete it
		if (mDefferedBuffer != nullptr)
			delete mDefferedBuffer;
		//Allocate new Deffered buffer
		mDefferedBuffer = allocFramebuffer(TargetWidth, TargetHeight);
		mDefferedBuffer->Usage = FB_USAGE_DEFERRED_PASS;
		//and bind new target texture
		mDefferedBuffer->AddTexture(mTarget->texture_ptr);
		mDefferedBuffer->AddTexture(FORMAT_RGBA); //Bloom map
		mDefferedBuffer->Create();

		//if gbuffer isn't created
		if (mGBuffer == nullptr) {
			//then create one
			mGBuffer = allocFramebuffer(TargetWidth, TargetHeight);
			mGBuffer->Usage = FB_USAGE_GBUFFER_PASS;
			mGBuffer->AddTexture(FORMAT_RGBA); //Diffuse map
			mGBuffer->AddTexture(FORMAT_RGB16F); //Normal map
			mGBuffer->AddTexture(FORMAT_RGB16F); //Position map
			mGBuffer->AddTexture(FORMAT_RGBA); //Specular map
			mGBuffer->AddTexture(FORMAT_RGBA); //Masks map
			mGBuffer->AddDepth();
			mGBuffer->Create();
		}
		//if already created, then just resize it
		else
			mGBuffer->SetSize(TargetWidth, TargetHeight);
	}
}

void Engine::CameraComponent::UpdateFramebuffers() {
	if (!FramebuffersCreated()) {
		if (mIsMainCamera) {
			mGBuffer = allocFramebuffer(TargetWidth, TargetHeight);
			mGBuffer->Usage = FB_USAGE_GBUFFER_PASS;
			
			mGBuffer->AddTexture(FORMAT_RGBA); //Diffuse map
			mGBuffer->AddTexture(FORMAT_RGB16F); //Normal map
			mGBuffer->AddTexture(FORMAT_RGB16F); //Position map
			mGBuffer->AddTexture(FORMAT_RGBA); //Specular map
			mGBuffer->AddTexture(FORMAT_RGBA); //Masks map
			mGBuffer->AddDepth();
			mGBuffer->Create();

			mDefferedBuffer = allocFramebuffer(TargetWidth, TargetHeight);
			mDefferedBuffer->Usage = FB_USAGE_DEFERRED_PASS;
			//and bind new target texture
			mDefferedBuffer->AddTexture(FORMAT_RGBA);
			mDefferedBuffer->AddTexture(FORMAT_RGBA); //Bloom map
			mDefferedBuffer->Create();
		}
		else {
			UpdateTextureResource();
		}
	}
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
	readBinaryValue(&mCullFaceDirection, data + offset, offset);

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
	_dest->mCullFaceDirection = this->mCullFaceDirection;
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
	stream->writeBinaryValue(&mCullFaceDirection);
}

void Engine::CameraComponent::onPreRender(Engine::Renderer* pipeline) {
	TransformProperty* transform = go_link.updLinkPtr()->getTransformProperty();

	float yaw = transform->abs_rotation.Y;
	float pitch = transform->abs_rotation.X;

	mCameraPos = transform->abs_translation;
	mCameraFront.X = cosf(DegToRad(yaw)) * cosf(DegToRad(pitch));
	mCameraFront.Y = -sinf(DegToRad(pitch));
	mCameraFront.Z = sinf(DegToRad(yaw)) * cosf(DegToRad(pitch));
	mCameraFront.Normalize();

	updateViewMat();

	
}

void Engine::CameraComponent::ResizeTarget(uint32_t Width, uint32_t Height) {

	if (Width == TargetWidth && Height == TargetHeight)
		return;

	TargetWidth = Width;
	TargetHeight = Height;

	if (mGBuffer != nullptr)
		mGBuffer->SetSize(Width, Height);
	if(mDefferedBuffer != nullptr)
		mDefferedBuffer->SetSize(Width, Height);
}

void Engine::CameraComponent::onObjectDeleted() {
	if (mDefferedBuffer != nullptr)
		delete mDefferedBuffer;
	if (mGBuffer != nullptr)
		delete mGBuffer;
}

void Engine::CameraComponent::bindObjectPropertyToAngel(AGScriptMgr* mgr) {
	mgr->RegisterObjectType(CAMERA_PROP_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);

	mgr->RegisterObjectMethod(CAMERA_PROP_TYPE_NAME, "void SetUp(const Vec3 &in)", asMETHOD(CameraComponent, setUp), asCALL_THISCALL);
	mgr->RegisterObjectMethod(CAMERA_PROP_TYPE_NAME, "void SetViewScale(const Vec3 &in)", asMETHOD(CameraComponent, setViewScale), asCALL_THISCALL);
	mgr->RegisterObjectMethod(CAMERA_PROP_TYPE_NAME, "void SetReflectionPlane(Plane &in)", asMETHOD(CameraComponent, setReflectionPlane), asCALL_THISCALL);

	mgr->RegisterObjectMethod(CAMERA_PROP_TYPE_NAME, "void SetAutoAspectRatio(bool)", asMETHOD(CameraComponent, SetAutoAspectRatio), asCALL_THISCALL);
	mgr->RegisterObjectMethod(CAMERA_PROP_TYPE_NAME, "void SetAspectRatio(float)", asMETHOD(CameraComponent, SetAspectRatio), asCALL_THISCALL);
	mgr->RegisterObjectMethod(CAMERA_PROP_TYPE_NAME, "float GetAspectRatio()", asMETHOD(CameraComponent, getAspectRatio), asCALL_THISCALL);
	mgr->RegisterObjectMethod(CAMERA_PROP_TYPE_NAME, "Vec3 GetUp()", asMETHOD(CameraComponent, GetUp), asCALL_THISCALL);
	mgr->RegisterObjectMethod(CAMERA_PROP_TYPE_NAME, "Vec3 GetFront()", asMETHOD(CameraComponent, GetFront), asCALL_THISCALL);


	mgr->RegisterObjectMethod(CAMERA_PROP_TYPE_NAME, "void SetViewMask(uint64)", asMETHOD(CameraComponent, SetViewMask), asCALL_THISCALL);

	mgr->RegisterObjectMethod(CAMERA_PROP_TYPE_NAME, "void UpdateProjectionMatrix()", asMETHOD(CameraComponent, UpdateProjectionMatrix), asCALL_THISCALL);
}