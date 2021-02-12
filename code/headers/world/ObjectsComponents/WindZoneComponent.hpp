#pragma once

#include "../World.hpp"

namespace Engine {
    class WindZoneComponent : public Engine::IGameObjectComponent {
    public:
        float mStrength;

        void addPropertyInterfaceToInspector();
        void onUpdate(float deltaTime);
        void copyTo(Engine::IGameObjectComponent* dest);
        void onPreRender(Engine::Renderer* pipeline);

        void loadPropertyFromMemory(const char* data, GameObject* obj);
        void savePropertyToStream(ZsStream* stream, GameObject* obj);

        Vec3 GetDirection();

        WindZoneComponent();
    };
}