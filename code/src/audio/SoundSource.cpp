#include "../../headers/audio/SoundSource.hpp"

#include <AL/al.h>
#include <AL/alc.h>

Engine::SoundSource::SoundSource() :
    source_gain(1.0f),
    source_pitch(1.0f)
{
    Init();
}

void Engine::SoundSource::Init() {
    alGenSources(1, &this->al_source_id);

    int err = alGetError();
    if (err != AL_NO_ERROR)
    {
        //std::cout << "Error creating source " << err << std::endl;

    }
}
void Engine::SoundSource::Destroy() {
    alDeleteSources(1, &this->al_source_id);
}
void Engine::SoundSource::apply_settings() {
    alSource3f(al_source_id, AL_POSITION, this->source_pos.X, this->source_pos.Y, this->source_pos.Z);
    alSourcef(al_source_id, AL_GAIN, this->source_gain);
    alSourcef(al_source_id, AL_PITCH, this->source_pitch);
    alSourcei(al_source_id, AL_LOOPING, static_cast<int>(this->looped));
    setVelocity(Vec3(0.0f, 0.0f, 0.0f));
}
void Engine::SoundSource::setPosition(Vec3 pos) {
    this->source_pos = pos;
    alSource3f(al_source_id, AL_POSITION, pos.X, pos.Y, pos.Z);
}

void Engine::SoundSource::setVelocity(Vec3 vel) {
    alSource3f(al_source_id, AL_VELOCITY, vel.X, vel.Y, vel.Z);
}

void Engine::SoundSource::play() {
    alSourcePlay(this->al_source_id);
}
void Engine::SoundSource::stop() {
    alSourceStop(this->al_source_id);
}
void Engine::SoundSource::pause() {
    alSourcePause(this->al_source_id);
}
void Engine::SoundSource::setAlBuffer(SoundBuffer* buffer) {
    stop();
    alSourcei(al_source_id, AL_BUFFER, static_cast<ALint>(buffer->getBufferIdAL()));
}
