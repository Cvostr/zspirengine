#pragma once

#include <string>
#include "../engine/BackgroundLoader.hpp"

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
	class Texture {
	public:

		//Only for OGL : initialize texture in GL
        virtual void Init(){}
		//Loads texture from buffer
        virtual bool LoadDDSTextureFromBuffer(unsigned char* data) { return false; }
		//Loads texture from file
        bool LoadDDSTextureFromFile(const char* path);
		//Use in rendering pipeline
        virtual void Use(int slot){}
        virtual void Destroy(){}

        Texture();
        virtual ~Texture();
	};

    class Texture3D{
    public:

        bool created;

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