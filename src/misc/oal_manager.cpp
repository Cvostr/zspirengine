#include <AL/al.h>
#include <AL/alc.h>

#include <iostream>
#include <fstream>

#include "../../headers/misc/oal_manager.h"

static ALCdevice* al_device;
static ALCcontext* al_context;

bool Engine::SFX::initAL() {
    al_device = alcOpenDevice(nullptr);
	if (!al_device) {
		std::cout << "AL: Can't initialize OpenAL device" << std::endl;
		return false;
	}

    al_context = alcCreateContext(al_device, nullptr);

	if (!al_context) {
		std::cout << "AL: Can't initialize OpenAL device context" << std::endl;
		return false;
	}

	alcMakeContextCurrent(al_context);

    if (alGetError() != AL_NO_ERROR)
    {
        fprintf(stderr, "Can't initialize");
    }

	std::cout << "AL: OpenAL successfully initialized!" << std::endl;
	//Set default parameters
	setListenerPos(ZSVECTOR3(0.0f, 0.0f, 0.0f));
    setListenerOri(ZSVECTOR3(0.0f, 0.0f, 1.0f), ZSVECTOR3(0.0f, 1.0f, 0.0f));
    setListenerVelocity(ZSVECTOR3(0.0f, 0.0f, 0.0f));
    Engine::SFX::setListenerVolume(1.0f);

	return true;
}

void Engine::SFX::setListenerPos(ZSVECTOR3 pos) {
    alListener3f(AL_POSITION, pos.X, pos.Y, pos.Z);
}
void Engine::SFX::setListenerOri(ZSVECTOR3 front, ZSVECTOR3 up) {
    ALfloat listenerOri[] = { front.X, front.Y, front.Z, up.X, up.Y, up.Z};
    alListenerfv(AL_ORIENTATION, listenerOri);
}

void Engine::SFX::setListenerVolume(float value){
	alListenerf(AL_GAIN, value);
}

void Engine::SFX::setListenerVelocity(ZSVECTOR3 velocity){
    alListener3f(AL_VELOCITY, velocity.X, velocity.Y, velocity.Z);
}

void Engine::SFX::destroyAL(){
    alcMakeContextCurrent(nullptr);
	alcDestroyContext(al_context);
	alcCloseDevice(al_device);
}

void Engine::SoundBuffer::Init(){
    alGenBuffers(1, &this->al_buffer_id);
}

bool Engine::SoundBuffer::loadBufferWAV(unsigned char* buffer){
    //std::cout << "Loading WAVE sound file " << file_path << std::endl;

    Init();

    unsigned int freq;
    ALenum format = AL_FORMAT_MONO16;
    unsigned int channels = 1;
    int bits;

    if (buffer[12] != 'f' || buffer[13] != 'm' || buffer[14] != 't' || buffer[15] != ' ')
    {
        return false;
    }
    if (buffer[21] != 0 || buffer[20] != 1)
    {
        return false;
    }

    channels = static_cast<unsigned int>(buffer[23] << 8);
    channels |= buffer[22];

    freq = static_cast<unsigned int>(buffer[27] << 24);
    freq |= static_cast<unsigned int>(buffer[26] << 16);
    freq |= static_cast<unsigned int>(buffer[25] << 8);
    freq |= static_cast<unsigned int>(buffer[24]);

    bits = buffer[35] << 8;
    bits |= buffer[34];

    if (bits == 8)
    {
        if (channels == 1)
            format = AL_FORMAT_MONO8;
        else if (channels == 2)
            format = AL_FORMAT_STEREO8;
    }
    else if (bits == 16)
    {
        if (channels == 1)
            format = AL_FORMAT_MONO16;
        else if (channels == 2)
            format = AL_FORMAT_STEREO16;
    }
    if (!format)
    {
        std::cout << "Incompatible format (" << channels << ", " << bits << ") :(" << std::endl;
        return false;
    }

    int start_offset = 35;

    //read 4 bytes, until "data" header found
    while(buffer[start_offset] != 'd' || buffer[start_offset + 1] != 'a' || buffer[start_offset + 2] != 't' || buffer[ start_offset + 3] != 'a')
        start_offset += 1;
        //Read size
    start_offset += 4;
    //Calculate size
    int _size = buffer[start_offset + 3] << 24; //Getting size, 32 bit value
    _size |= buffer[start_offset + 2] << 16;
    _size |= buffer[start_offset + 1] << 8;
    _size |= buffer[start_offset];

   // for(int i = 0; i < 4;i++)
   //     std::cout << static_cast<int>(buffer[start_offset + i]) << std::endl;

    start_offset += 4;

    //Send data to OpenAL
    alBufferData(this->al_buffer_id, format, static_cast<void*>(buffer + start_offset), _size, static_cast<int>(freq));
    int err = alGetError();
    if (err != AL_NO_ERROR)
    {
        std::cout << "Error loading " << err << std::endl;
        return false;
    }

    return true;
}

void Engine::SoundBuffer::Destroy(){
    alDeleteBuffers(1, &this->al_buffer_id);
}
unsigned int Engine::SoundBuffer::getBufferIdAL(){
    return this->al_buffer_id;
}
Engine::SoundBuffer::SoundBuffer(){

}

void Engine::SoundSource::Init(){
    alGenSources(1, &this->al_source_id);

    int err = alGetError();
    if (err != AL_NO_ERROR)
    {
        std::cout <<  "Error creating source " << err << std::endl;

    }
}
void Engine::SoundSource::Destroy(){
    alDeleteSources(1, &this->al_source_id);
}
void Engine::SoundSource::apply_settings(){
    alSource3f(al_source_id, AL_POSITION, this->source_pos.X, this->source_pos.Y, this->source_pos.Z);
    alSourcef(al_source_id, AL_GAIN, this->source_gain);
    alSourcef(al_source_id, AL_PITCH, this->source_pitch);
    alSourcei(al_source_id, AL_LOOPING, static_cast<int>(this->looped));
    setVelocity(ZSVECTOR3(0.0f, 0.0f, 0.0f));
}
void Engine::SoundSource::setPosition(ZSVECTOR3 pos){
    this->source_pos = pos;
    alSource3f(al_source_id, AL_POSITION, pos.X, pos.Y, pos.Z);
}

void Engine::SoundSource::setVelocity(ZSVECTOR3 vel){
    alSource3f(al_source_id, AL_VELOCITY, vel.X, vel.Y, vel.Z);
}

void Engine::SoundSource::play(){
    alSourcePlay(this->al_source_id);
}
void Engine::SoundSource::stop(){
    alSourceStop(this->al_source_id);
}
void Engine::SoundSource::pause(){
    alSourcePause(this->al_source_id);
}
void Engine::SoundSource::setAlBuffer(SoundBuffer* buffer){
    stop();
    alSourcei(al_source_id, AL_BUFFER, static_cast<ALint>(buffer->getBufferIdAL()));
}
