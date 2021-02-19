
#include "../../headers/vulkan/ZSVulkanInstance.hpp"
#include <iostream>

Engine::ZSVulkanInstance::ZSVulkanInstance(){
    this->window_ptr = nullptr;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}


const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

std::vector<const char*> extensions = {
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
   
    //,"VK_EXT_debug_report"
};

bool Engine::ZSVulkanInstance::init(bool validate, const char* app_name, int app_ver, SDL_Window* window){
   
    unsigned int ext_count;
    if (!SDL_Vulkan_GetInstanceExtensions(window, &ext_count, nullptr)) return false;


    size_t additional_extension_count = extensions.size();
    extensions.resize(additional_extension_count + ext_count);

    if (!SDL_Vulkan_GetInstanceExtensions(window, &ext_count, extensions.data() + additional_extension_count)) return false;

    VkApplicationInfo vk_app_info = {};
    vk_app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    vk_app_info.pNext = nullptr;
    vk_app_info.apiVersion = VK_MAKE_VERSION(1, 2, 0);
    vk_app_info.pEngineName = "ZSPIRE";
    vk_app_info.engineVersion = 1;
    vk_app_info.pApplicationName = app_name;
    vk_app_info.applicationVersion = static_cast<unsigned int>(app_ver);

    VkInstanceCreateInfo vk_inst_create_info = {};
    vk_inst_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    vk_inst_create_info.pNext = nullptr;
    vk_inst_create_info.pApplicationInfo = &vk_app_info;
    vk_inst_create_info.enabledExtensionCount = static_cast<unsigned int>(extensions.size());
    vk_inst_create_info.ppEnabledExtensionNames = extensions.data();
    if (validate) {
        vk_inst_create_info.enabledLayerCount = static_cast<unsigned int>(validationLayers.size());
        vk_inst_create_info.ppEnabledLayerNames = validationLayers.data();
    }

    std::cout << "Creating Vulkan Instance" << std::endl;
    if (vkCreateInstance(&vk_inst_create_info, nullptr, &this->mInstance) != VK_SUCCESS) {
        std::cout << "Can't create Vulkan Instance. Terminating" << std::endl;
        return false;
    }

    if (!SDL_Vulkan_CreateSurface(window, mInstance, &mSurface))
        return false;

    if (validate) {
        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr; // Optional

        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(mInstance, "vkCreateDebugUtilsMessengerEXT");


        if (func(this->mInstance, &createInfo, nullptr, &this->debugMessenger) != VK_SUCCESS) {
            std::cout << "Can't create Vulkan debug callback" << std::endl;
        }
    }

    return true;
}
