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
        void Transition(VmaVkBuffer temp, unsigned int MipLevel, uint32_t Width, uint32_t Height);
        void CreateImageView(VkFormat format);
    public:

        VkImageUsageFlagBits usage;
        VkImageAspectFlagBits aspect;

        VkImageView GetImageView() { return mImageView; }

        void Create(unsigned int Width, unsigned int Height, TextureFormat format, unsigned int Layers = 1);
        //Loads texture from buffer
        bool LoadDDSTextureFromBuffer(unsigned char* data);
        //Use in rendering pipeline
        void Use(int slot);
        void Destroy();

        vkTexture();
        ~vkTexture();
    };

    VkFormat GetFormatVK(TextureFormat format);

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