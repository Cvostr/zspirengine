#pragma once

#include <vulkan/vulkan.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vector>

#include "../engine/Window.hpp"

namespace Engine {

    class ZSVulkanInstance {
    private:
        VkDebugUtilsMessengerEXT debugMessenger;

        SDL_Window* window_ptr;

        VkInstance mInstance;

        std::vector<VkExtensionProperties> inst_extensions;

        VkSurfaceKHR mSurface;

    public:
        bool init(bool validate, const char* app_name, int app_ver, SDL_Window* window);

        VkInstance GetInstance() { return mInstance; }
        VkSurfaceKHR GetSurface() { return mSurface; }

        ZSVulkanInstance();
    };

}