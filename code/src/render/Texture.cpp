#include "../../headers/game.h"
#include <render/Texture.h>

#include <ogl/GLTexture.hpp>
#include <vulkan/VKTexture.hpp>

extern ZSpireEngine* engine_ptr;
//Hack to support resources
extern ZSGAME_DATA* game_data;

Engine::Texture* Engine::allocTexture(){
    Engine::Texture* result = nullptr;
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL: {
            result = new glTexture;
            break;
        }
        case VULKAN : {
            result = new vkTexture;
            break;
        }
    }
    return result;
}

Engine::Texture3D* Engine::allocTexture3D(){
    Engine::Texture3D* result = nullptr;
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL: {
            result = new glTexture3D;
            break;
        }
        case VULKAN : {
            result = new vkTexture3D;
            break;
        }
    }
    return result;
}

bool Engine::Texture::LoadFromBuffer(unsigned char* data, uint32_t size) {
    bool result = true;
    if (data[0] == 'D' && data[1] == 'D' && data[2] == 'S') {
        result = LoadDDSTextureFromBuffer(data);
    }
    if (data[0] == '\x89' && data[1] == 'P' && data[2] == 'N' && data[3] == 'G') {
        LoadPNGTextureFromBuffer(data, (int)size);
    }
    if (data[0] == 'R' && data[1] == 'T' && data[2] == 'B' ) {
        if (engine_ptr->engine_info->graphicsApi == VULKAN) {
            vkTexture* vkt = static_cast<vkTexture*>(this);

            vkt->usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            vkt->aspect = VK_IMAGE_ASPECT_COLOR_BIT;
        }
        Create(512, 512, TextureFormat::FORMAT_RGBA);
        SetRenderTargetFlag(true, true);
    }
    return result;
}

//Loads texture from file
bool Engine::Texture::LoadDDSTextureFromFile(const char* path){

    std::ifstream texture_stream;
    texture_stream.open(path, std::ifstream::binary | std::ifstream::ate);

    if (texture_stream.fail()) { //Opening file stream failed, no file
        //std::cout << "TEXTURE: FATAL: Error opening file stream! Perhaps, file " << path << " is missing!" << std::endl;
        return false;
    }

    //texture file size
    unsigned int size = static_cast<unsigned int>(texture_stream.tellg());
    texture_stream.seekg(0);

    unsigned char header[128];
    //reading texture header
    texture_stream.read(reinterpret_cast<char*>(&header[0]), 128);

    if (header[0] != 'D' && header[1] != 'D' && header[2] != 'S') { //File found, but isn't DDS texture
        //std::cout << "TEXTURE: FATAL: Error processing file! Perhaps, file " << path << " is not DDS texture!" << std::endl;
        texture_stream.close();
        return false;
    }
    //back to start
    texture_stream.seekg(0);

    unsigned char * data = new unsigned char[size]; //Allocating buffer for file in heap
    texture_stream.read(reinterpret_cast<char*>(data), size); //Reading file to buffer
    LoadDDSTextureFromBuffer(data); //Read texture from buffer

    delete[] (data); //freeing buffer
    texture_stream.close(); //closing stream

    return true;
}

bool Engine::Texture::LoadPNGTextureFromFile(const char* path) {
    std::ifstream stream(path, std::ios_base::binary | std::ios_base::ate);

    if (stream.fail()) { //Opening file stream failed, no file
        //std::cout << "TEXTURE: FATAL: Error opening file stream! Perhaps, file " << path << " is missing!" << std::endl;
        return false;
    }
    unsigned int size = static_cast<unsigned int>(stream.tellg());
    stream.seekg(0);

    char* data = new char[size];
    stream.read(data, size);
    LoadPNGTextureFromBuffer((unsigned char*)data, size);
    delete[] data;
    stream.close();

    return true;
}

Engine::Texture::Texture() :
    mMipsCount(0),
    mLayers(1) {}

Engine::Texture::~Texture(){

}

bool Engine::Texture3D::pushTexture(int index, std::string path){
    //return false;
    if(index > 5 || path.compare("@none") == 0) return false;

    this->units[index].path = path;
    this->units[index].req = new Engine::Loader::LoadRequest;
    //Check, if we use engine from Editor
    if (!game_data->isEditor) {
        //We are not in editor
        TextureResource* texture = game_data->resources->getTextureByLabel(path);
        this->units[index].req->offset = texture->offset;
        //Set size of resource in blob
        this->units[index].req->size = texture->size;
        this->units[index].req->file_path = texture->blob_path;
    }
    else {
        TextureResource* texture = game_data->resources->getTextureByLabel(path);

        //We are in editor
        this->units[index].req->size = 0;
        this->units[index].req->file_path = texture->rel_path;
        this->units[index].req->offset = 0;
    }
    //Load binary file multithreaded
    Engine::Loader::loadImmideately(this->units[index].req);
    if (!pushTextureBuffer(index, this->units[index].req->data))
        return false;
    //Free bytes
    delete[] this->units[index].req->data;
    delete this->units[index].req;

    return true;
}


Engine::Texture3D::Texture3D(){

}
Engine::Texture3D::~Texture3D(){

}
