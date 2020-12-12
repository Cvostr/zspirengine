#include "EngineComponent.h"
#include <SDL2/SDL.h>

namespace Engine {

	class Time : public IEngineComponent {
	private:
		uint64_t mLast;
		uint64_t mNow;
	public:
		Time() {
			mNow = SDL_GetPerformanceCounter();
			mLast = 0;
		}
		void Tick() {
			mLast = mNow;
			mNow = SDL_GetPerformanceCounter();
		}
		float GetDeltaTime() {
			return (float)(mNow - mLast) / 1000.f;
		}
	};

}