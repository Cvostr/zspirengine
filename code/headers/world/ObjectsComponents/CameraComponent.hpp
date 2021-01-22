#pragma once

#include "../World.hpp"
#include "../Camera.hpp"
#include "../../render/Texture.h"

namespace Engine {
	class CameraComponent : public IGameObjectComponent, Camera {
	public:
		bool mIsMainCamera;
		Texture* mTarget;

		void addPropertyInterfaceToInspector();
		void onValueChanged(); //Update soud buffer pointer and send source props

		void onPreRender(Engine::Renderer* pipeline);
		void onUpdate(float deltaTime);
		void loadPropertyFromMemory(const char* data, GameObject* obj);
		void savePropertyToStream(ZsStream* stream, GameObject* obj);
		//void bindObjectPropertyToAngel(AGScriptMgr* mgr);

		CameraComponent();
	};
}