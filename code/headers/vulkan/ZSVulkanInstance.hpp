#pragma once

#include <vulkan/vulkan.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vector>

#include "../engine/engine_window.h"

class ZsVulkanInstance{
private:
    VkDebugUtilsMessengerEXT debugMessenger;

    SDL_Window* window_ptr;

    VkInstance instance;

    std::vector<VkExtensionProperties> inst_extensions;
    std::vector<VkPhysicalDevice> phys_devices_list;
    std::vector<VkPhysicalDeviceProperties> phys_devices_props;

    VkPhysicalDevice selected_device;
    VkDevice logicalDevice; //logical device, created from physical device

    //ZsVkFamilyIndices q_f_indices; //stores queue indices
    VkQueue graphicsQueue; //queue of logical device
    VkQueue presentQueue; //queue to present


public:
    bool init(bool validate, const char* app_name, int app_ver, SDL_Window* window);
    bool initDevice(bool validate);

    VkDevice getVkDevice();
    VkPhysicalDevice getPhysicalDevice();
    VkQueue getGraphicsQueue();
    VkQueue getPresentQueue();

    ZsVulkanInstance();
};
