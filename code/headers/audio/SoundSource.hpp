#pragma once

#include "../math/Vec3.hpp"
#include "SoundBuffer.hpp"

namespace Engine {

    class SoundSource {
    private:
        unsigned int al_source_id;
    public:

        SoundSource();

        Vec3 source_pos; //Position of audio source
        float source_gain;
        float source_pitch;
        bool looped;

        void Init();
        void Destroy();
        void setPosition(Vec3 pos);
        void setVelocity(Vec3 vel);
        void pause();
        void apply_settings();
        void play();
        void stop();
        void setAlBuffer(SoundBuffer* buffer);
    };

}