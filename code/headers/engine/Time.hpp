#include "EngineComponent.h"
#include <SDL2/SDL.h>
#include "../Scripting/AngelScriptMgr.h"

namespace Engine {

	class Time : public IEngineComponent {
	private:
		uint64_t mLast;
		uint64_t mNow;
		uint64_t mFramesCount;
		float DeltaTime;
	public:
		Time();
		void Tick();
		float GetDeltaTime();
		uint64_t GetFramesCount();

		static void bindAngelScript(Engine::AGScriptMgr* mgr);
	};

}