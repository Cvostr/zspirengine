#pragma once

namespace Engine {

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
}