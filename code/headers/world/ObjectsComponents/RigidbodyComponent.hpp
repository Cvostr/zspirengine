#pragma once

#include "../World.hpp"
#include <math/Vec3.hpp>
#include "PhysicalComponent.hpp"

namespace Engine {

    class RigidbodyProperty : public Engine::PhysicalProperty {
    public:
        Vec3 gravity;
        Vec3 linearVel;
        Vec3 angularVel;

        void addPropertyInterfaceToInspector();
        void onUpdate(float deltaTime);
        void onStart();
        void copyTo(Engine::IGameObjectComponent* dest);
        void onValueChanged();
        void onObjectDeleted(); //unregister in world

        void setLinearVelocity(Vec3& lvel);
        void applyCentralImpulse(Vec3& imp);
        void applyCentralForce(Vec3& v);

        void loadPropertyFromMemory(const char* data, GameObject* obj);
        void savePropertyToStream(ZsStream* stream, GameObject* obj);
        void bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr);

        RigidbodyProperty();
    };
}