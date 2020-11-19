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


bool Engine::SoundBuffer::loadFileWAV(const char* file_path){
    std::cout << "Loading WAVE sound file " << file_path << std::endl;

    Init();
    unsigned int freq;
    ALenum format = AL_FORMAT_MONO16;
    unsigned int channels = 1;
    int bits;

    unsigned char* data_buffer;

    std::ifstream audio_stream;
    audio_stream.open(file_path, std::iostream::binary | std::iostream::ate);

    if (audio_stream.fail()) return false;

    int size = static_cast<int>(audio_stream.tellg());
    data_buffer = new unsigned char[size];

    audio_stream.seekg(0);

    audio_stream.read(reinterpret_cast<char*>(data_buffer), 12);

    audio_stream.read(reinterpret_cast<char*>(data_buffer), 8);
    if (data_buffer[0] != 'f' || data_buffer[1] != 'm' || data_buffer[2] != 't' || data_buffer[3] != ' ')
    {
        delete [] data_buffer; //Free heap
        audio_stream.close();
        return false;
    }
    audio_stream.read(reinterpret_cast<char*>(data_buffer), 2);
    if (data_buffer[1] != 0 || data_buffer[0] != 1)
    {
        delete [] data_buffer; //Free heap
        audio_stream.close();
        fprintf(stderr, "Not PCM :(\n"); //Close stream
        return false;
    }

    audio_stream.read(reinterpret_cast<char*>(data_buffer), 2);
    channels = static_cast<unsigned int>(data_buffer[1] << 8);
    channels |= data_buffer[0];

    audio_stream.read(reinterpret_cast<char*>(data_buffer), 4);
    freq = static_cast<unsigned int>(data_buffer[3] << 24);
    freq |= static_cast<unsigned int>(data_buffer[2] << 16);
    freq |= static_cast<unsigned int>(data_buffer[1] << 8);
    freq |= static_cast<unsigned int>(data_buffer[0]);

    audio_stream.read(reinterpret_cast<char*>(data_buffer), 6);
    audio_stream.read(reinterpret_cast<char*>(data_buffer), 2);

    bits = data_buffer[1] << 8;
    bits |= data_buffer[0];

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
        delete [] data_buffer; //Free heap
        audio_stream.close();
        std::cout << "Incompatible format (" << channels << ", " << bits << ") :(" << std::endl;
        return false;
    }
    //read 4 bytes, until "data" header found
    while(data_buffer[0] != 'd' || data_buffer[1] != 'a' || data_buffer[2] != 't' || data_buffer[3] != 'a')
        audio_stream.read(reinterpret_cast<char*>(data_buffer), 4);
    //Read size
    audio_stream.read(reinterpret_cast<char*>(data_buffer), 4);
    //Calculate size
    int _size = data_buffer[3] << 24; //Getting size, 32 bit value
    _size |= data_buffer[2] << 16;
    _size |= data_buffer[1] << 8;
    _size |= data_buffer[0];
    //read data
    audio_stream.read(reinterpret_cast<char*>(data_buffer), _size);
    //Send data to OpenAL
    alBufferData(this->al_buffer_id, format, static_cast<void*>((data_buffer)), _size, static_cast<int>(freq));
    int err = alGetError();
    if (err != AL_NO_ERROR)
    {
        std::cout << "Error loading " << err << std::endl;
        delete [] data_buffer; //Free heap
        audio_stream.close();
        return false;
    }

    delete [] data_buffer; //Free heap
    audio_stream.close();
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


Engine::SoundSource::SoundSource() {
    source_gain = 1.0f;
    source_pitch = 1.0f;

    Init();
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
