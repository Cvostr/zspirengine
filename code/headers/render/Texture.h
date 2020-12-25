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
        FORMAT_RGBA16F
    };

	class Texture : public GpuObject{
    protected:
        unsigned int maxWidth;
        unsigned int maxHeight;
	public:

		//Only for OGL : initialize texture in GL
        virtual void Init(){}
        virtual void Create(unsigned int Width, unsigned int Height, TextureFormat format){}
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