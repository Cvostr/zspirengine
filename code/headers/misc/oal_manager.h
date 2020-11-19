#pragma once

#include "../render/Math.hpp"

namespace Engine {
	namespace SFX {
		//Initializes sound OpenAL device to begin work
		bool initAL();
		//Closes initialized device
		void destroyAL();

		void setListenerPos(ZSVECTOR3 pos);
        void setListenerOri(ZSVECTOR3 front, ZSVECTOR3 up);
        void setListenerVelocity(ZSVECTOR3 velocity);
		void setListenerVolume(float value);


	}


class SoundBuffer {
private:
     unsigned int al_buffer_id;
public:
     void Init();
     bool loadBufferWAV(unsigned char* buffer);
     bool loadFileWAV(const char* file_path);
     void Destroy();
     unsigned int getBufferIdAL();
     SoundBuffer();
};

class SoundSource{
private:
    unsigned int al_source_id;
public:

    SoundSource();

    ZSVECTOR3 source_pos; //Position of audio source
    float source_gain;
    float source_pitch;
    bool looped;

    void Init();
    void Destroy();
    void setPosition(ZSVECTOR3 pos);
    void setVelocity(ZSVECTOR3 vel);
    void pause();
    void apply_settings();
    void play();
    void stop();
    void setAlBuffer(SoundBuffer* buffer);
};
}
