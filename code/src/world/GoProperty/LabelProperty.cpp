#include "../../../headers/world/World.hpp"


Engine::LabelProperty::LabelProperty() {
    this->type = PROPERTY_TYPE::GO_PROPERTY_TYPE_LABEL;
}

Engine::LabelProperty::~LabelProperty() {

}

void Engine::LabelProperty::copyTo(Engine::IGameObjectComponent* dest) {
    if (dest->type != this->type) return; //if it isn't label

    //Do base things
    IGameObjectComponent::copyTo(dest);

    LabelProperty* _dest = static_cast<LabelProperty*>(dest);
    _dest->label = label;
}


void Engine::LabelProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
    unsigned int offset = 1;
    readString(this->label, data, offset);
}

void Engine::LabelProperty::savePropertyToStream(ZsStream* stream, GameObject* obj) {
    stream->writeString(this->label);
}