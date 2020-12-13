#include "EngineComponent.h"
#include <SDL2/SDL.h>

namespace Engine {

	class Time : public IEngineComponent {
	private:
		uint64_t mLast;
		uint64_t mNow;
		uint64_t mFramesCount;
	public:
		Time() {
			mNow = SDL_GetPerformanceCounter();
			mLast = 0;
			mFramesCount = 0;
		}
		void Tick() {
			mFramesCount = 0;
			mLast = mNow;
			mNow = SDL_GetPerformanceCounter();
		}
		float GetDeltaTime() {
			return (float)(mNow - mLast) / 1000.f;
		}
		uint64_t GetFramesCount() {
			return mFramesCount;
		}
	};

}