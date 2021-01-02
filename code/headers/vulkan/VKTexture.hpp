#pragma once

#include "ZSVMA.hpp"
#include "../render/Texture.h"
#include <vulkan/vulkan.hpp>

namespace Engine {
    class vkTexture : public Engine::Texture {
    private:
        VkImage mImage;
        VkImageView mImageView;
        VkDeviceMemory mImageMem;
        void Transition(VmaVkBuffer temp);
        void CreateImageView(VkFormat format);
    public:

        VkImageView GetImageView() { return mImageView; }
        void Init();
        //Loads texture from buffer
        bool LoadDDSTextureFromBuffer(unsigned char* data);
        //Use in rendering pipeline
        void Use(int slot);
        void Destroy();

        vkTexture();
        ~vkTexture();
    };


    class vkTexture3D : public Engine::Texture3D {
    public:

        void Init();
        bool pushTextureBuffer(int index, unsigned char* data);
        //Use in rendering pipeline
        void Use(int slot);
        void Destroy();

        vkTexture3D();
        ~vkTexture3D();
    };

}