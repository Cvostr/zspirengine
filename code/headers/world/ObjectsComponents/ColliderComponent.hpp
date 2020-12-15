#pragma once

#include "../World.hpp"
#include "PhysicalComponent.hpp"

namespace Engine {
    class ColliderProperty : public Engine::PhysicalProperty {
    public:
        void onObjectDeleted(); //unregister in world
        void addPropertyInterfaceToInspector();
        void copyTo(Engine::IGameObjectComponent* dest);
        void onUpdate(float deltaTime);
        void onStart();
        Engine::TransformProperty* getTransformProperty();

        void loadPropertyFromMemory(const char* data, GameObject* obj);
        void savePropertyToStream(ZsStream* stream, GameObject* obj);
        void bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr);

        ColliderProperty();
    };
}