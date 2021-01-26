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
        void Create(unsigned int Width, unsigned int Height, TextureFormat format, unsigned int Layers = 1);
        //Loads texture from buffer
        bool LoadDDSTextureFromBuffer(unsigned char* data);
        bool LoadPNGTextureFromBuffer(unsigned char* data, int size);
        bool LoadTextureFromBufferUByte(unsigned char* data, int Width, int Height, TextureFormat format);
        //Use in rendering pipeline
        void Use(int slot);
        void Destroy();
        GLenum GetGlType() {
            return mLayers > 1 ? GL_TEXTURE_2D_ARRAY :  GL_TEXTURE_2D;
        }

        glTexture();
        ~glTexture();
    };

    class glTexture3D : public Texture3D {
    public:
        unsigned int TEXTURE_ID;
        unsigned int mLayers;

        void Init();
        void Create(unsigned int Width, unsigned int Height, unsigned int Layers, TextureFormat format);
        bool pushTextureBuffer(int index, unsigned char* data);
        //Use in rendering pipeline
        void Use(int slot);
        void Destroy();

        glTexture3D();
        ~glTexture3D();
    };

}