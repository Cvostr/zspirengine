#pragma once

#include <engine/EngineComponentManager.hpp>
#include <string>
#include <engine/Resources.hpp>

namespace Engine {
	class WorldLoadManager : public IEngineComponent {
	private:
		Engine::SceneResource* mScheduledSceneResource;
		bool mResourceScheduled;
		bool mCallOnStartOnLoad;
	public:

		void OnUpdate();
		void ScheduleWorldLoad(std::string WorldLabel);

		WorldLoadManager() : 
			mResourceScheduled(false),
			mCallOnStartOnLoad(true) {}
	};
}