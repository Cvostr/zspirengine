#ifndef zs_texture
#define zs_texture

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII
#define SLOTS_COUNT 4

namespace Engine {
	class Texture {
	public:

		//Only for OGL : initialize texture in GL
        virtual void Init();
		//Loads texture from buffer
        virtual bool LoadDDSTextureFromBuffer(unsigned char* data);
		//Loads texture from file
        bool LoadDDSTextureFromFile(const char* path);
		//Use in rendering pipeline
        virtual void Use(int slot);
        virtual void Destroy();

        Texture();
        virtual ~Texture();
	};

    Texture* allocTexture();
}



#endif
