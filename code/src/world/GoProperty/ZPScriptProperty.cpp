#include "../../../headers/world/go_properties.h"

extern ZSGAME_DATA* game_data;

Engine::ZPScriptProperty::ZPScriptProperty() {
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_SCRIPTGROUP;
    script = nullptr;
    script_res = nullptr;
}

Engine::ZPScriptProperty::~ZPScriptProperty() {

}
void Engine::ZPScriptProperty::onStart() {
    if (script_res != nullptr) {
        script = new AGScript(game_data->script_manager, go_link.updLinkPtr(), "angel");

        script->compileFromResource(this->script_res);
        script->onStart();
    }
}
void Engine::ZPScriptProperty::onUpdate(float deltaTime) {
    if (script_res != nullptr) {
        script->onUpdate();
    }
}

void Engine::ZPScriptProperty::copyTo(Engine::GameObjectProperty* dest) {

}

void Engine::ZPScriptProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {

}
