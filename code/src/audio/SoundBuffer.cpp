#include "../../headers/audio/SoundBuffer.hpp"

#include <AL/al.h>
#include <AL/alc.h>
#include <fstream>

#include "../../headers/engine/Logger.hpp"

void Engine::SoundBuffer::Init() {
    alGenBuffers(1, &this->al_buffer_id);
}

bool Engine::SoundBuffer::loadBufferWAV(unsigned char* buffer) {
    //Logger::Log(LogType::LOG_TYPE_INFO) << "Loading WAVE sound file " << file_path << std::endl;

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
        Logger::Log(LogType::LOG_TYPE_ERROR) << "Incompatible format (" << channels << ", " << bits << ")\n";
        return false;
    }

    int start_offset = 35;

    //read 4 bytes, until "data" header found
    while (buffer[start_offset] != 'd' || buffer[start_offset + 1] != 'a' || buffer[start_offset + 2] != 't' || buffer[start_offset + 3] != 'a')
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
        Logger::Log(LogType::LOG_TYPE_ERROR) << "Error loading " << err << "\n";
        return false;
    }

    return true;
}


bool Engine::SoundBuffer::loadFileWAV(const char* file_path) {
    Logger::Log(LogType::LOG_TYPE_INFO) << "Loading WAVE sound file " << file_path << "\n";

    Init();

    char* data_buffer;

    std::ifstream audio_stream;
    audio_stream.open(file_path, std::iostream::binary | std::iostream::ate);

    if (audio_stream.fail()) return false;

    int size = static_cast<int>(audio_stream.tellg());
    data_buffer = new char[size];

    audio_stream.seekg(0);
    audio_stream.read(data_buffer, size);

    loadBufferWAV((unsigned char*)data_buffer);

    delete[] data_buffer; //Free heap
    audio_stream.close();
    return true;
}

void Engine::SoundBuffer::Destroy() {
    alDeleteBuffers(1, &this->al_buffer_id);
}
unsigned int Engine::SoundBuffer::getBufferIdAL() {
    return this->al_buffer_id;
}
Engine::SoundBuffer::SoundBuffer(): al_buffer_id(0) {}
