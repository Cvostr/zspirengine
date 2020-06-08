#include "../../headers/vulkan/vk_data.h"

void Engine::_vk_Texture::Init(){

}
//Loads texture from buffer
bool Engine::_vk_Texture::LoadDDSTextureFromBuffer(unsigned char* data){
    return true;
}
//Use in rendering pipeline
void Engine::_vk_Texture::Use(int slot){

}
void Engine::_vk_Texture::Destroy(){

}

Engine::_vk_Texture::_vk_Texture(){

}
Engine::_vk_Texture::~_vk_Texture(){

}



void Engine::_vk_Texture3D::Init(){

}
bool Engine::_vk_Texture3D::pushTextureBuffer(int index, unsigned char* data){
    return true;
}
//Use in rendering pipeline
void Engine::_vk_Texture3D::Use(int slot){

}
void Engine::_vk_Texture3D::Destroy(){

}

Engine::_vk_Texture3D::_vk_Texture3D(){

}
Engine::_vk_Texture3D::~_vk_Texture3D(){

}
