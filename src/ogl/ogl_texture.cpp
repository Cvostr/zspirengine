#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include "../../headers/ogl/ogl.h"


void Engine::_ogl_Texture::Init() {
    glGenTextures(1, &this->TEXTURE_ID); //Initializing texture in GL
    glBindTexture(GL_TEXTURE_2D, this->TEXTURE_ID); //We now working with this texture

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
}

Engine::_ogl_Texture::_ogl_Texture() {

}

Engine::_ogl_Texture::~_ogl_Texture(){

}

void Engine::_ogl_Texture::Use(int slot) {
    glActiveTexture(GL_TEXTURE0 + slot); //Activating texture slot
    glBindTexture(GL_TEXTURE_2D, this->TEXTURE_ID); //Sending texture to activated slot
}

void Engine::_ogl_Texture::Destroy() {

    glDeleteTextures(1, &this->TEXTURE_ID);

#ifdef ZS_LOG
    std::cout << "TEXTURE: realease sucess!" << std::endl;
#endif
}

bool Engine::_ogl_Texture::LoadDDSTextureFromBuffer(unsigned char* data){

    Init();

    int HEIGHT = *(reinterpret_cast<int*>(&(data[12]))); //Getting height of texture in px info
    int WIDTH = *(reinterpret_cast<int*>(&(data[16]))); //Getting width of texture in px info
    unsigned int linearSize = *(reinterpret_cast<unsigned int*>(&(data[20])));
    unsigned int mipMapCount = *(reinterpret_cast<unsigned int*>(&(data[28])));
    unsigned int fourCC = *(reinterpret_cast<unsigned int*>(&(data[84])));

    unsigned char * bufferT;
    unsigned int bufsize;

    bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;//Getting buffer size

    bufferT = data + 128; //jumping over header


    unsigned int format; //Getting texture format
    switch (fourCC)
    {
    case FOURCC_DXT1:
        format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        break;
    case FOURCC_DXT3:
        format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        break;
    case FOURCC_DXT5:
        format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        break;
    default:

        return 0;
    }
    //Getting block size
    unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    unsigned int offset = 0;

    int nwidth = WIDTH;
    int nheight = HEIGHT;

    //Mipmaps
    for (unsigned int level = 0; level < mipMapCount && (nwidth || nheight); ++level) //Iterating over mipmaps
    {
        unsigned int size = ((nwidth + 3) / 4)*((nheight + 3) / 4)*blockSize; //Calculating mip texture size
        glCompressedTexImage2D(GL_TEXTURE_2D, level, format, nwidth, nheight,
            0, size, bufferT + offset);

        offset += size;
        nwidth /= 2;
        nheight /= 2;
    }


    return true;
}
