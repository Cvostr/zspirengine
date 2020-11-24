#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include "../../headers/ogl/ogl.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

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

bool Engine::_ogl_Texture::LoadPNGTextureFromBuffer(unsigned char* data, int size) {
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load_from_memory (data, size, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    Init();

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload pixels into texture
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image_data);

    return true;
}

void Engine::_ogl_Texture3D::Init(){
    glGenTextures(1, &this->TEXTURE_ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->TEXTURE_ID);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}
bool Engine::_ogl_Texture3D::pushTextureBuffer(int index, unsigned char* data){
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->TEXTURE_ID);

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
        glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + index, level, format, nwidth, nheight,
            0, size, bufferT + offset);

        offset += size;
        nwidth /= 2;
        nheight /= 2;
    }
    return true;
}

//Use in rendering pipeline
void Engine::_ogl_Texture3D::Use(int slot){
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, TEXTURE_ID);
}
void Engine::_ogl_Texture3D::Destroy(){
    glDeleteTextures(1, &TEXTURE_ID);
}
Engine::_ogl_Texture3D::_ogl_Texture3D(){

}
Engine::_ogl_Texture3D::~_ogl_Texture3D(){

}
