#pragma once

#include "../World.hpp"

namespace Engine {

    class SkyboxProperty : public Engine::IGameObjectComponent {
    public:
        void onPreRender(Engine::Renderer* pipeline);
        void DrawSky(Renderer* pipeline);
        SkyboxProperty();
    };
}