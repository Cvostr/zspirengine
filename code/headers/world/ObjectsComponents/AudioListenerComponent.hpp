#pragma once

#include "../World.hpp"

namespace Engine {
    class AudioListenerComponent : public IGameObjectComponent {
    public:
    
        void onPreRender(Engine::Renderer* pipeline);

        AudioListenerComponent();
    };
}