#pragma once

#include "../World.hpp"
#include "../Camera.hpp"
#include "../../../headers/engine/Resources.hpp"

namespace Engine {

	class CameraComponent : public IGameObjectComponent, Camera {
	private:
		std::string TargetResourceName;
	public:
		bool mIsMainCamera;
		bool mAutoViewport;
		ViewMask mViewMask;
		CameraCullFaceDirection mCullFaceDirection;
		TextureResource* mTarget;

		void addPropertyInterfaceToInspector();
		void onValueChanged(); //Update soud buffer pointer and send source props

		void UpdateTextureResource();
		void onPreRender(Engine::Renderer* pipeline);
		void onUpdate(float deltaTime);
		void copyTo(Engine::IGameObjectComponent* dest);
		void loadPropertyFromMemory(const char* data, GameObject* obj);
		void savePropertyToStream(ZsStream* stream, GameObject* obj);
		void bindObjectPropertyToAngel(AGScriptMgr* mgr);

		void setReflectionPlane(Plane& plane) {
			Camera::setReflectionPlane(plane);
		}

		void setUp(const Vec3& up) {
			Camera::setUp(up);
		}

		void setViewScale(const Vec3& scale) {
			Camera::setViewScale(scale);
		}

		void SetViewMask(ViewMask Mask) {
			mViewMask = Mask; }

		CameraComponent();
	};
}