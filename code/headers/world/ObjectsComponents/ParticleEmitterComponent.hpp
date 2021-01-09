#pragma once

#include "../World.hpp"

namespace Engine {
    class ParticleEmitterComponent : public Engine::IGameObjectComponent {
    public:

        void addPropertyInterfaceToInspector();
        void onUpdate(float deltaTime);
        void copyTo(Engine::IGameObjectComponent* dest);
        void onObjectDeleted(); //unregister in world
        void onStart();

        void loadPropertyFromMemory(const char* data, GameObject* obj);
        void savePropertyToStream(ZsStream* stream, GameObject* obj);


        ParticleEmitterComponent();
    };
}