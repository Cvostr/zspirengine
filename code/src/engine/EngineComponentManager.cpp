#include <engine/EngineComponentManager.hpp>

using namespace Engine;

EngineComponentManager::EngineComponentManager() {

}

void EngineComponentManager::startManager(IEngineComponent* component) {
    component->OnCreate();
    this->components.push_back(component);
}

void EngineComponentManager::CallOnUpdate() {
    for (unsigned int i = 0; i < static_cast<unsigned int>(components.size()); i++) {
        components[i]->OnUpdate();
    }
}

void EngineComponentManager::destroyAllManagers() {
    //we must do that in reverse order
    for (int i = static_cast<int>(components.size()) - 1; i >= 0; i--) {
        delete components[i];
    }
    components.clear();
}

void EngineComponentManager::updateResolution(int W, int H) {
    for (unsigned int i = 0; i < static_cast<unsigned int>(components.size()); i++) {
        components[i]->OnUpdateWindowSize(W, H);
    }
}