#ifndef zsvulkan
#define zsvulkan

#include "vulkan/vulkan.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vector>


#include "../engine/engine_window.h"

class ZsVulkan;

typedef struct SwapChainSupportDetails{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
}SwapChainSupportDetails;

typedef struct ZsVkFamilyIndices{
    uint32_t graphicsIndex;
    uint32_t presentIndex;
}ZsVkFamilyIndices;

typedef struct ZsVkPipelineConf{
    VkPrimitiveTopology iaTopology;
    VkViewport viewport;
    VkRect2D scissor;
    VkPolygonMode polygonMode;

    unsigned int cullFace;

    ZsVkPipelineConf(){
        iaTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        polygonMode = VK_POLYGON_MODE_FILL;
        cullFace = VK_CULL_MODE_NONE;
    }
}ZsVkPipelineConf;

class ZsVkPipeline{
private:
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;
public:
    VkPipeline getPipeline();

    void create(void* shader, ZsVkPipelineConf conf, ZsVulkan* vulkan);
};

class ZsVulkan{
private:
    VkDebugUtilsMessengerEXT debugMessenger;

    SDL_Window* window_ptr;

    VkInstance instance;

    std::vector<VkExtensionProperties> inst_extensions;
    std::vector<VkPhysicalDevice> phys_devices_list;
    std::vector<VkPhysicalDeviceProperties> phys_devices_props;

    VkPhysicalDevice selected_device;
    VkDevice logicalDevice; //logical device, created from physical device

    ZsVkFamilyIndices q_f_indices; //stores queue indices
    VkQueue graphicsQueue; //queue of logical device
    VkQueue presentQueue; //queue to present

    VkSurfaceKHR vk_surface;
    VkSwapchainKHR vk_swapchain;

    std::vector<VkImageView> swapChainImageViews;
public:
    bool init(bool validate, const char* app_name, int app_ver, SDL_Window* window, ZSWINDOW_CREATE_INFO* win_info);
    bool initDevice(bool validate);
    void initSurface();
    bool initSwapChain(ZSWINDOW_CREATE_INFO* win_info);

    VkDevice getVkDevice();

    SwapChainSupportDetails getSwapChainDetails();

    ZsVulkan();
};



#endif
