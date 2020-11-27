#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include "../math/Vec3.hpp"
#include "../engine/EngineComponent.h"

namespace Engine {
	class OALManager : public IEngineComponent {
	public:
		OALManager();
		~OALManager();

		//Initializes sound OpenAL device to begin work
		bool initAL();
		void OnCreate();
		//Closes initialized device
		void destroyAL();

		void setListenerPos(Vec3 pos);
		void setListenerOri(Vec3 front, Vec3 up);
		void setListenerVelocity(Vec3 velocity);
		void setListenerVolume(float value);
	private:
		ALCdevice* mOpenALDevice;
		ALCcontext* mOpenALContext;

	};

}