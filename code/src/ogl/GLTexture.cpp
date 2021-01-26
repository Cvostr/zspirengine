#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include "../../headers/ogl/GLTexture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

using namespace Engine;

GLint Engine::GetInternalFormatGL(TextureFormat format) {
    GLint gl_format = 0;
    switch (format) {
    case TextureFormat::FORMAT_R:
        gl_format = GL_R8;
        break;
    case TextureFormat::FORMAT_RG:
        gl_format = GL_RG8;
        break;
    case TextureFormat::FORMAT_RGB:
        gl_format = GL_RGB8;
        break;
    case TextureFormat::FORMAT_RGBA:
        gl_format = GL_RGBA8;
        break;
    case TextureFormat::FORMAT_R16:
        gl_format = GL_R16;
        break;
    case TextureFormat::FORMAT_RG16:
        gl_format = GL_RG16;
        break;
    case TextureFormat::FORMAT_RGB16:
        gl_format = GL_RGB16;
        break;
    case TextureFormat::FORMAT_RGBA16:
        gl_format = GL_RGBA16;
        break;
    case TextureFormat::FORMAT_R16F:
        gl_format = GL_R16F;
        break;
    case TextureFormat::FORMAT_RG16F:
        gl_format = GL_RG16F;
        break;
    case TextureFormat::FORMAT_RGB16F:
        gl_format = GL_RGB16F;
        break;
    case TextureFormat::FORMAT_RGBA16F:
        gl_format = GL_RGBA16F;
        break;
    case TextureFormat::FORMAT_DEPTH_24_STENCIL_8:
        gl_format = GL_DEPTH24_STENCIL8;
        break;
    case TextureFormat::FORMAT_DEPTH_32:
        gl_format = GL_DEPTH_COMPONENT32;
        break;
    }

    return gl_format;
}
GLenum Engine::GetFormatGL(TextureFormat format) {
    GLint gl_format = 0;

    if (format % 4 == 0)
        gl_format = GL_RGBA;
    if (format % 4 == 1)
        gl_format = GL_RED;
    if (format % 4 == 2)
        gl_format = GL_RG;
    if (format % 4 == 3)
        gl_format = GL_RGB;

    if (format == TextureFormat::FORMAT_DEPTH_24_STENCIL_8)
        gl_format = GL_DEPTH_STENCIL;
    if (format == TextureFormat::FORMAT_DEPTH_32)
        gl_format = GL_DEPTH_COMPONENT;

    return gl_format;
}

void glTexture::Init() {

    GLenum Type = GetGlType();

    if (!mCreated) {
        glCreateTextures(Type, 1, &TEXTURE_ID);
        glBindTexture(Type, this->TEXTURE_ID); //We now working with this texture

        glTexParameteri(Type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(Type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(Type, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(Type, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

        mCreated = true;
    }
}

void glTexture::Create(unsigned int Width, unsigned int Height, TextureFormat format, unsigned int Layers) {
    mLayers = Layers;
    maxWidth = Width;
    maxHeight = Height;

    mFormat = format;

    GLenum _type = GL_UNSIGNED_BYTE;

    if (format == TextureFormat::FORMAT_DEPTH_24_STENCIL_8)
        _type = GL_UNSIGNED_INT_24_8;

    if (format == TextureFormat::FORMAT_DEPTH_32)
        _type = GL_FLOAT;

    GLenum TextureType = GetGlType();

    Init();
    if(Layers == 1)
        glTexImage2D(GL_TEXTURE_2D, 0, GetInternalFormatGL(format), Width, Height, 0, GetFormatGL(format), _type, nullptr);
    else if (Layers > 1)
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GetInternalFormatGL(format), Width, Height, Layers, 0, GetFormatGL(format), _type, nullptr);
    
}

glTexture::glTexture():
    TEXTURE_ID(0)
{
    mCreated = false;
}

glTexture::~glTexture(){
    Destroy();
}

void glTexture::Use(int slot) {
    glBindTextureUnit(slot, TEXTURE_ID);
}

void glTexture::Destroy() {
    if (mCreated) {
        glDeleteTextures(1, &this->TEXTURE_ID);
        mCreated = false;
    }
}

bool glTexture::LoadTextureFromBufferUByte(unsigned char* data, int Width, int Height, TextureFormat format) {
    Init();

    glBindTexture(GL_TEXTURE_2D, this->TEXTURE_ID);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GetInternalFormatGL(format),
        Width,
        Height,
        0,
        GetFormatGL(format),
        GL_UNSIGNED_BYTE,
        data
    );

    mFormat = format;
    maxWidth = Width;
    maxHeight = Height;

    return true;
}

bool glTexture::LoadDDSTextureFromBuffer(unsigned char* data){

    Init();

    maxHeight = *(reinterpret_cast<int*>(&(data[12]))); //Getting height of texture in px info
    maxWidth = *(reinterpret_cast<int*>(&(data[16]))); //Getting width of texture in px info
    unsigned int linearSize = *(reinterpret_cast<unsigned int*>(&(data[20])));
    mMipsCount = *(reinterpret_cast<unsigned int*>(&(data[28])));
    unsigned int fourCC = *(reinterpret_cast<unsigned int*>(&(data[84])));

    unsigned char * bufferT;
    unsigned int bufsize;

    bufsize = mMipsCount > 1 ? linearSize * 2 : linearSize;//Getting buffer size

    bufferT = data + 128; //jumping over header


    unsigned int format; //Getting texture format
    switch (fourCC)
    {
    case FOURCC_DXT1:
        format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        mFormat = FORMAT_BC1_UNORM;
        break;
    case FOURCC_DXT3:
        format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        mFormat = FORMAT_BC2_UNORM;
        break;
    case FOURCC_DXT5:
        format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        mFormat = FORMAT_BC3_UNORM;
        break;
    default:

        return 0;
    }
    //Getting block size
    unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    unsigned int offset = 0;

    int nwidth = maxWidth;
    int nheight = maxHeight;

    //Mipmaps
    for (unsigned int level = 0; level < mMipsCount && (nwidth || nheight); ++level) //Iterating over mipmaps
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

bool glTexture::LoadPNGTextureFromBuffer(unsigned char* data, int size) {
    // Load from file
    int image_width = 0;
    int image_height = 0;
    mFormat = FORMAT_RGBA;
    unsigned char* image_data = stbi_load_from_memory (data, size, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    maxWidth = image_width;
    maxHeight = image_height;

    // Create a OpenGL texture identifier
    Init();

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload pixels into texture
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GetInternalFormatGL(mFormat), image_width, image_height, 0, GetFormatGL(mFormat), GL_UNSIGNED_BYTE, image_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image_data);

    return true;
}

void glTexture3D::Init(){
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &TEXTURE_ID);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}
bool glTexture3D::pushTextureBuffer(int index, unsigned char* data){
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
void glTexture3D::Use(int slot){
    glBindTextureUnit(slot, TEXTURE_ID);
}
void glTexture3D::Destroy(){
    glDeleteTextures(1, &TEXTURE_ID);
}
Engine::glTexture3D::glTexture3D() :
    TEXTURE_ID(0)
{

}
Engine::glTexture3D::~glTexture3D(){

}
