#ifndef zsvulkan
#define zsvulkan

#include "vulkan/vulkan.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vector>

class ZsVulkan{
private:
    SDL_Window* window_ptr;

    VkInstance instance;
    VkPhysicalDevice used_gpu;

    std::vector<VkExtensionProperties> inst_extensions;
    std::vector<VkPhysicalDevice> phys_devices_list;
    std::vector<VkPhysicalDeviceProperties> phys_devices_props;

    VkPhysicalDevice selected_device;
    VkDevice logicalDevice; //logical device, created from physical device

    VkQueue graphicsQueue; //queue of logical device
    VkQueue presentQueue; //queue to present

    VkSurfaceKHR vk_surface;
public:
    bool init(const char* app_name, int app_ver, SDL_Window* window);
    void initDevice();
    void initSurface();
    ZsVulkan();
};

#endif
