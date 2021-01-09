#pragma once

#include "../World.hpp"
#include "../../math/RGBColor.hpp"
#include "../../math/Vec3.hpp"

namespace Engine {

    enum class LIGHTSOURCE_TYPE {
        LIGHTSOURCE_TYPE_NONE,
        LIGHTSOURCE_TYPE_DIRECTIONAL,
        LIGHTSOURCE_TYPE_POINT,
        LIGHTSOURCE_TYPE_SPOT
    };

    class LightsourceComponent : public Engine::IGameObjectComponent {
    public:
        LIGHTSOURCE_TYPE light_type; //type of lightsource

        Vec3 direction; //direction for directional & spotlight in quats
        //To compare differences
        Vec3 last_pos; //transform* last position
        Vec3 last_rot; //transform* last rotation

        RGBAColor color; //Color of light
        float intensity; //Light's intensity
        float range; //Light's range
        float spot_angle;

        void addPropertyInterfaceToInspector();
        void copyTo(Engine::IGameObjectComponent* dest);
        void onPreRender(Engine::Renderer* pipeline);
        void loadPropertyFromMemory(const char* data, GameObject* obj);
        void savePropertyToStream(ZsStream* stream, GameObject* obj);
        void bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr);

        LightsourceComponent();
    };
}