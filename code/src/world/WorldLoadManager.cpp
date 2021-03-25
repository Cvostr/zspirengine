#include <world/WorldLoadManager.hpp>
#include <game.h>
#include <world/World.hpp>

extern ZSGAME_DATA* game_data;

void Engine::WorldLoadManager::OnUpdate() {
	if (mResourceScheduled && mScheduledSceneResource) {
		mResourceScheduled = false;
		mScheduledSceneResource->load();
		game_data->world->loadFromMemory((const char*)mScheduledSceneResource->data, mScheduledSceneResource->size, game_data->pipeline->getRenderSettings());
	
		if (mCallOnStartOnLoad) {
			game_data->world->call_onStart();
		}
	}
}
void Engine::WorldLoadManager::ScheduleWorldLoad(std::string WorldLabel) {
	if (!mResourceScheduled) {
		mScheduledSceneResource = game_data->resources->getResource<SceneResource>(WorldLabel);
		mResourceScheduled = true;
	}
}