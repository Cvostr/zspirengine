#pragma once

#include <string>
#include "../engine/BackgroundLoader.hpp"
#include "GpuObject.hpp"

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII
#define SLOTS_COUNT 4

typedef struct Texture3D_Unit{

    std::string path;
    Engine::Loader::LoadRequest* req;

    Texture3D_Unit(){
        req = nullptr;
    }

}Texture3D_Unit;

namespace Engine {

    enum TextureFormat {
        FORMAT_R = 1,
        FORMAT_RG,
        FORMAT_RGB,
        FORMAT_RGBA,

        FORMAT_R16,
        FORMAT_RG16,
        FORMAT_RGB16,
        FORMAT_RGBA16,

        FORMAT_R16F,
        FORMAT_RG16F,
        FORMAT_RGB16F,
        FORMAT_RGBA16F,

        FORMAT_BC1_UNORM,
        FORMAT_BC2_UNORM,
        FORMAT_BC3_UNORM,

        FORMAT_DEPTH_24_STENCIL_8 = 120,
        FORMAT_DEPTH_32
    };

	class Texture : public GpuObject{
    protected:
        uint32_t maxWidth;
        uint32_t maxHeight;
        TextureFormat mFormat;
        uint32_t mMipsCount;
        uint32_t mLayers;
	public:
        TextureFormat GetFormat() { return mFormat; }
        uint32_t GetMipsCount() { return mMipsCount; }
        uint32_t GetLayersCount() { return mLayers; }
        uint32_t GetWidth() { return maxWidth; }
        uint32_t GetHeight() { return maxHeight; }
		//Only for OGL : initialize texture in GL
        virtual void Create(uint32_t Width, uint32_t Height, TextureFormat format, uint32_t Layers = 1){}
		//Loads texture from buffer
        virtual bool LoadDDSTextureFromBuffer(unsigned char* data) { return false; }
        virtual bool LoadPNGTextureFromBuffer(unsigned char* data, int size) { return false; }
        virtual bool LoadTextureFromBufferUByte(unsigned char* data, int Width, int Height, TextureFormat format) { return false; }
		//Loads texture from file
        bool LoadDDSTextureFromFile(const char* path);
        bool LoadPNGTextureFromFile(const char* path);
		//Use in rendering pipeline
        virtual void Use(int slot){}

        Texture();
        virtual ~Texture();
	};

    class Texture3D : public GpuObject {
    public:

        Texture3D_Unit units[6];

        virtual void Init(){}
        virtual bool pushTextureBuffer(int index, unsigned char* data) { return true; }
        virtual bool pushTexture(int index, std::string path);
        //Use in rendering pipeline
        virtual void Use(int slot){}
        virtual void Destroy(){}

        Texture3D();
        virtual ~Texture3D();
    };

    Texture* allocTexture();
    Texture3D* allocTexture3D();
}