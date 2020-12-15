#pragma once

#include "../World.hpp"
#include "PhysicalComponent.hpp"
#include "../../math/Vec3.hpp"

namespace Engine {

    class CharacterControllerProperty : public Engine::PhysicalProperty {
    private:
        bool _isOnGround;
    public:
        float width;
        float height;

        Vec3 gravity;
        Vec3 linearVel;
        Vec3 angularVel;

        void addPropertyInterfaceToInspector();
        void copyTo(Engine::IGameObjectComponent* dest);
        void onUpdate(float deltaTime);
        void onStart();

        void setLinearVelocity(Vec3 lvel);

        bool isOnGround();
        void jump(float height);
        void walk(float x, float z);

        void loadPropertyFromMemory(const char* data, GameObject* obj);
        void savePropertyToStream(ZsStream* stream, GameObject* obj);
        void bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr);

        CharacterControllerProperty();
    };

}