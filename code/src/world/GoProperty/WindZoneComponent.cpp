#include "../../../headers/world/ObjectsComponents/WindZoneComponent.hpp"

void Engine::WindZoneComponent::onUpdate(float deltaTime) {

}

void Engine::WindZoneComponent::onPreRender(Engine::Renderer* pipeline) {
    pipeline->addWind(pipeline);
}

void Engine::WindZoneComponent::copyTo(Engine::IGameObjectComponent* dest) {
    if (dest->type != this->type) return; //if it isn't Lightsource, then exit

    //Do base things
    IGameObjectComponent::copyTo(dest);

    WindZoneComponent* _dest = static_cast<WindZoneComponent*>(dest);
    _dest->mStrength = mStrength;
}

void Engine::WindZoneComponent::loadPropertyFromMemory(const char* data, GameObject* obj) {
    unsigned int offset = 1;
    //Read Type of Light Source
    readBinaryValue(&mStrength, data + offset, offset);
}
void Engine::WindZoneComponent::savePropertyToStream(ZsStream* stream, GameObject* obj) {
    stream->writeBinaryValue(&mStrength);
}

Vec3 Engine::WindZoneComponent::GetDirection() {
	TransformProperty* transform = go_link.updLinkPtr()->getTransformProperty();

	return _getDirection(transform->abs_rotation.X, transform->abs_rotation.Y, transform->abs_rotation.Z);
}

Engine::WindZoneComponent::WindZoneComponent() {
	type = PROPERTY_TYPE::GO_PROPERTY_TYPE_WINDZONE;
}