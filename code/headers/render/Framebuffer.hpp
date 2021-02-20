#pragma once

#include "GL/glew.h"
#include "Texture.h"
#include "GpuObject.hpp"

#define MAX_RENDERER_ATTACHMENT_COUNT 8

namespace Engine {
    //Useful for Vulkan to describe renderpass
    enum FramebufferUsage {
        FB_USAGE_DEFAULT,
        FB_USAGE_GBUFFER_PASS,
        FB_USAGE_DEFERRED_PASS
    };

    class Framebuffer : public GpuObject{
    protected:
        uint32_t Width;
        uint32_t Height;

        uint32_t mTexturesCount;

        Texture* textures[MAX_RENDERER_ATTACHMENT_COUNT];
        Texture* depthTexture;

        bool Depth;

    public:
        FramebufferUsage Usage;

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
    
        virtual void AddTexture(Texture* Texture) {}

        virtual void Create(){}

        explicit Framebuffer() :
            Width (0),
            Height(0),
            mTexturesCount(0),
            Depth(false),
            depthTexture(nullptr),
            Usage(FB_USAGE_DEFAULT)
        {
            textures[0] = 0;
        }

        virtual ~Framebuffer() {

        }
    };

    Framebuffer* allocFramebuffer();
    Framebuffer* allocFramebuffer(uint32_t Width, uint32_t Height);

}