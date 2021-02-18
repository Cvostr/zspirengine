#include <world/ObjectsComponents/AudioListenerComponent.hpp>
#include <world/ObjectsComponents/CameraComponent.hpp>
#include <game.h>

extern ZSGAME_DATA* game_data;

void Engine::AudioListenerComponent::onPreRender(Engine::Renderer* pipeline) {
	TransformProperty* transform = go_link.updLinkPtr()->getTransformProperty();
	CameraComponent* camera = go_link.updLinkPtr()->getPropertyPtr<CameraComponent>();

	if (transform == nullptr || camera == nullptr)
		return;

	Vec3 Pos = transform->abs_translation;
	Vec3 Front = camera->GetFront();
	Vec3 Up = camera->GetUp();

	game_data->oal_manager->setListenerPos(Pos);
	game_data->oal_manager->setListenerOri(Front, Up);
}

Engine::AudioListenerComponent::AudioListenerComponent() {
	type = PROPERTY_TYPE::GO_PROPERTY_TYPE_AUDIO_LISTENER;
}