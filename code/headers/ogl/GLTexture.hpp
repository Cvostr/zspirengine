#pragma once

#include "../render/Texture.h"
#include <GL/glew.h>

namespace Engine {

    GLenum GetFormatGL(TextureFormat format);
    GLint GetInternalFormatGL(TextureFormat format);

    class glTexture : public Texture {
    public:

        unsigned int TEXTURE_ID;

        //Only for OGL : initialize texture in GL
        void Init();
        void Create(unsigned int Width, unsigned int Height, TextureFormat format);
        //Loads texture from buffer
        bool LoadDDSTextureFromBuffer(unsigned char* data);
        bool LoadPNGTextureFromBuffer(unsigned char* data, int size);
        bool LoadTextureFromBufferUByte(unsigned char* data, int Width, int Height, TextureFormat format);
        //Use in rendering pipeline
        void Use(int slot);
        void Destroy();

        glTexture();
        ~glTexture();
    };

    class glTexture3D : public Texture3D {
    public:
        unsigned int TEXTURE_ID;

        void Init();
        bool pushTextureBuffer(int index, unsigned char* data);
        //Use in rendering pipeline
        void Use(int slot);
        void Destroy();

        glTexture3D();
        ~glTexture3D();
    };

}