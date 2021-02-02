#pragma once

#include "GL/glew.h"
#include "Texture.h"
#include "GpuObject.hpp"

#define MAX_RENDERER_ATTACHMENT_COUNT 7

namespace Engine {
    class Framebuffer : public GpuObject{
    protected:
        uint32_t Width;
        uint32_t Height;

        uint32_t mTexturesCount;

        Texture* textures[MAX_RENDERER_ATTACHMENT_COUNT];
        Texture* depthTexture;

        bool Depth;

    public:
        Texture* GetDepthTexture() { return depthTexture; }
        Texture* GetTexture(unsigned int i) { return textures[i]; }

        uint32_t GetWidth() { return Width; }
        uint32_t GetHeight() { return Height; }
        uint32_t GetTexturesCount() { return mTexturesCount; }

        //Recreate all textures with specified size
        virtual void SetSize(uint32_t Width, uint32_t Height)
        {
            this->Width = Width;
            this->Height = Height;
        }

        virtual void AddTexture(TextureFormat Format = TextureFormat::FORMAT_RGBA){}
        virtual void AddDepth(unsigned int Layers = 1, TextureFormat Format = FORMAT_DEPTH_24_STENCIL_8){}

        virtual void AddTexture(uint32_t Width, uint32_t Height, TextureFormat Format) {}
        virtual void AddDepth(uint32_t Width, uint32_t Height, unsigned int Layers = 1, TextureFormat Format = FORMAT_DEPTH_24_STENCIL_8) {}
    
        virtual void Create(){}
    };

    Framebuffer* allocFramebuffer();
    Framebuffer* allocFramebuffer(uint32_t Width, uint32_t Height);

}