#include "../../headers/engine/EngineComponentManager.hpp"

using namespace Engine;

EngineComponentManager::EngineComponentManager() {

}

void EngineComponentManager::startManager(IEngineComponent* component) {
    component->OnCreate();
    this->components.push_back(component);
}

void EngineComponentManager::destroyAllManagers() {
    //we must do that in reverse order
    for (int i = static_cast<int>(components.size()) - 1; i >= 0; i--) {
        delete components[i];
    }
}

void EngineComponentManager::updateResolution(int W, int H) {
    for (unsigned int i = 0; i < components.size(); i++) {
        components[i]->OnUpdateWindowSize(W, H);
    }
}