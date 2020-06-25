#include "../../headers/game.h"
#include "../../headers/render/zs-texture.h"

#include "../../headers/ogl/ogl.h"
#include "../../headers/vulkan/vk_data.h"

extern ZSpireEngine* engine_ptr;
//Hack to support resources
extern ZSGAME_DATA* game_data;

Engine::Texture* Engine::allocTexture(){
    Engine::Texture* result = nullptr;
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL32 : {
            result = new _ogl_Texture;
            break;
        }
        case VULKAN : {
            result = new _vk_Texture;
            break;
        }
    }
    return result;
}

Engine::Texture3D* Engine::allocTexture3D(){
    Engine::Texture3D* result = nullptr;
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL32 : {
            result = new _ogl_Texture3D;
            break;
        }
        case VULKAN : {
            result = new _vk_Texture3D;
            break;
        }
    }
    return result;
}

//Only for OGL : initialize texture in GL
void Engine::Texture::Init(){
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL32 : {
            static_cast<_ogl_Texture*>(this)->Init();
            break;
        }
        case VULKAN : {
            static_cast<_vk_Texture*>(this)->Init();
            break;
        }
    }
}
//Loads texture from buffer
bool Engine::Texture::LoadDDSTextureFromBuffer(unsigned char* data){
    bool result = false;
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL32 : {
            result = static_cast<_ogl_Texture*>(this)->LoadDDSTextureFromBuffer(data);
            break;
        }
        case VULKAN : {
            result = static_cast<_vk_Texture*>(this)->LoadDDSTextureFromBuffer(data);
            break;
        }
    }
    return result;
}
//Loads texture from file
bool Engine::Texture::LoadDDSTextureFromFile(const char* path){

#ifdef ZS_LOG
    std::cout << "TEXTURE: Loading texture from file : " << path << std::endl;
#endif

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
//Use in rendering pipeline
void Engine::Texture::Use(int slot){
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL32 : {
            static_cast<_ogl_Texture*>(this)->Use(slot);
            break;
        }
        case VULKAN : {
            static_cast<_vk_Texture*>(this)->Use(slot);
            break;
        }
    }
}
void Engine::Texture::Destroy(){
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL32 : {
            static_cast<_ogl_Texture*>(this)->Destroy();
            break;
        }
        case VULKAN : {
            static_cast<_vk_Texture*>(this)->Destroy();
            break;
        }
    }
}

Engine::Texture::Texture(){

}
Engine::Texture::~Texture(){

}


void Engine::Texture3D::Init(){
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL32 : {
            static_cast<_ogl_Texture3D*>(this)->Init();
            break;
        }
        case VULKAN : {
            static_cast<_vk_Texture3D*>(this)->Init();
            break;
        }
    }
}
bool Engine::Texture3D::pushTextureBuffer(int index, unsigned char* data){
    bool result = false;
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL32 : {
            result = static_cast<_ogl_Texture3D*>(this)->pushTextureBuffer(index, data);
            break;
        }
        case VULKAN : {
            result = static_cast<_vk_Texture3D*>(this)->pushTextureBuffer(index, data);
            break;
        }
    }
    return result;
}
bool Engine::Texture3D::pushTexture(int index, std::string path){
    if(index > 5) return false;

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
        //We are in editor
        this->units[index].req->size = 0;
        this->units[index].req->file_path = path;
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
    //Use in rendering pipeline
void Engine::Texture3D::Use(int slot){
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL32 : {
            static_cast<_ogl_Texture3D*>(this)->Use(slot);
            break;
        }
        case VULKAN : {
            static_cast<_vk_Texture3D*>(this)->Use(slot);
            break;
        }
    }
}
void Engine::Texture3D::Destroy(){
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL32 : {
            static_cast<_ogl_Texture3D*>(this)->Destroy();
            break;
        }
        case VULKAN : {
            static_cast<_vk_Texture3D*>(this)->Destroy();
            break;
        }
    }
}

Engine::Texture3D::Texture3D(){
    created = false;
}
Engine::Texture3D::~Texture3D(){

}