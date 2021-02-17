#pragma once

#include "../World.hpp"
#include "../Camera.hpp"
#include "../../render/Framebuffer.hpp"
#include "../../../headers/engine/Resources.hpp"

namespace Engine {

	class CameraComponent : public IGameObjectComponent, Camera {
	private:
		std::string TargetResourceName;

		uint32_t TargetWidth;
		uint32_t TargetHeight;
	public:
		bool mIsMainCamera;
		bool mAutoViewport;
		ViewMask mViewMask;
		CameraCullFaceDirection mCullFaceDirection;
		TextureResource* mTarget;

		Framebuffer* mGBuffer;
		Framebuffer* mDefferedBuffer;

		bool FramebuffersCreated() {
			return mGBuffer != nullptr && mDefferedBuffer != nullptr;
		}

		void addPropertyInterfaceToInspector();
		void onValueChanged(); //Update soud buffer pointer and send source props

		void UpdateTextureResource();
		void onPreRender(Engine::Renderer* pipeline);
		void onUpdate(float deltaTime);
		void copyTo(Engine::IGameObjectComponent* dest);
		void loadPropertyFromMemory(const char* data, GameObject* obj);
		void savePropertyToStream(ZsStream* stream, GameObject* obj);
		void bindObjectPropertyToAngel(AGScriptMgr* mgr);

		void ResizeTarget(uint32_t Width, uint32_t Height);
		void onObjectDeleted();

		void setReflectionPlane(Plane& plane) {
			Camera::setReflectionPlane(plane);
		}

		void setUp(const Vec3& up) {
			Camera::setUp(up);
		}

		void SetAspectRatio(float AspectRatio) { mAspectRatio = AspectRatio; }
		float getAspectRatio() { return mAspectRatio; }
		Vec3 GetFront() { return mCameraFront; }
		Vec3 GetUp() { return mCameraUp; }

		void setViewScale(const Vec3& scale) {
			Camera::setViewScale(scale);
		}

		void SetViewMask(ViewMask Mask) { mViewMask = Mask; }

		CameraComponent();
	};
}