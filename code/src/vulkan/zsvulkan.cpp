#include "../../headers/vulkan/zsvulkan.h"
#include "../../headers/vulkan/vk_data.h"
#include <iostream>

ZsVulkan* vulkan_ptr = nullptr;

ZsVulkan::ZsVulkan(){
    this->window_ptr = nullptr;
}

VkDevice ZsVulkan::getVkDevice(){
    return this->logicalDevice;
}

VkPhysicalDevice ZsVulkan::getPhysicalDevice(){
    return this->selected_device;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const std::vector<const char*> validationLayers = {
    "VK_LAYER_LUNARG_standard_validation"
};

std::vector<const char*> extensions = {
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
    "VK_EXT_debug_report"
};

bool ZsVulkan::init(bool validate, const char* app_name, int app_ver, SDL_Window* window, ZSWINDOW_CREATE_INFO* win_info){
   
    return true;
}

bool ZsVulkan::initDevice(bool validate){
    
    return true;
}

void ZsVulkan::initSurface(){
   
}

SwapChainSupportDetails ZsVulkan::getSwapChainDetails(){
    SwapChainSupportDetails result;
    //Get surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->selected_device, this->vk_surface, &result.capabilities);

    uint32_t surface_format_count = 0;
    //Get surface formats
    vkGetPhysicalDeviceSurfaceFormatsKHR(this->selected_device, this->vk_surface, &surface_format_count, nullptr);

    result.formats.resize(surface_format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(this->selected_device, this->vk_surface, &surface_format_count, result.formats.data());

    uint32_t present_mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(this->selected_device, this->vk_surface, &present_mode_count, nullptr);

    result.presentModes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(this->selected_device, this->vk_surface, &present_mode_count, result.presentModes.data());

    return result;
}

bool ZsVulkan::initSwapChain(ZSWINDOW_CREATE_INFO* win_info){
    
    return true;
}

bool ZsVkPipeline::create(void* shader, ZsVkPipelineConf conf, ZsVulkan* vulkan){
   
    return true;
}


VkPipeline ZsVkPipeline::getPipeline(){
    return this->pipeline;
}

VkRenderPass ZsVkPipeline::getRenderPass(){
    return this->renderPass;
}

ZsVkSwapchain* ZsVulkan::getSwapChain(){
    return &this->swapchain;
}

ZsVkFamilyIndices* ZsVulkan::getFamilyIndices(){
    return &this->q_f_indices;
}

VkQueue ZsVulkan::getGraphicsQueue(){
    return graphicsQueue;
}
VkQueue ZsVulkan::getPresentQueue(){
    return presentQueue;
}

VkFramebuffer ZsVkFrameBuffer::getFramebuffer(){
    return this->framebuffer;
}

bool ZsVkFrameBuffer::create(ZsVkPipeline* pipeline){
    
        return true;
}
