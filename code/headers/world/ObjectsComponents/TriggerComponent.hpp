#pragma once

#include "../World.hpp"
#include "PhysicalComponent.hpp"

namespace Engine {
    class TriggerProperty : public Engine::PhysicalProperty {
    private:
        SmArray<int>* _array;

    public:
        btGhostObject* m_ghost;

        void addPropertyInterfaceToInspector();
        void onUpdate(float deltaTime);
        void copyTo(Engine::IGameObjectComponent* dest);
        void initGhost();
        void onObjectDeleted(); //unregister in world
        void onStart();

        void loadPropertyFromMemory(const char* data, GameObject* obj);
        void savePropertyToStream(ZsStream* stream, GameObject* obj);

        TriggerProperty();
    };
}