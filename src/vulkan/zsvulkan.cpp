#include "../../headers/vulkan/zsvulkan.h"
#include <iostream>

ZsVulkan::ZsVulkan(){
 //Nothing much to do
}

bool ZsVulkan::init(const char* app_name, int app_ver){
    uint32_t num_extensions;
    vkEnumerateInstanceExtensionProperties(nullptr, &num_extensions, nullptr);
    this->inst_properties.resize(num_extensions);
    vkEnumerateInstanceExtensionProperties(nullptr, &num_extensions, this->inst_extensions.data());

    VkApplicationInfo vk_app_info = {};
    vk_app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    vk_app_info.pNext = nullptr;
    vk_app_info.apiVersion = VK_API_VERSION_1_0;
    vk_app_info.pEngineName = "PERSH_ZSPIRE";
    vk_app_info.engineVersion = 0;
    vk_app_info.pApplicationName = app_name;
    vk_app_info.applicationVersion = app_ver;

    VkInstanceCreateInfo vk_inst_create_info = {};
    vk_inst_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    vk_inst_create_info.pNext = nullptr;
    vk_inst_create_info.pApplicationInfo = &vk_app_info;
    //vk_inst_create_info.enabledExtensionCount = num_extensions;
    //vk_inst_create_info.ppEnabledExtensionNames

    std::cout << "Creating Vulkan Instance" << std::endl;
    if(vkCreateInstance(&vk_inst_create_info, nullptr, &this->instance) != VK_SUCCESS){
        std::cout << "Can't create Vulkan Instance. Terminating" << std::endl;
        return false;
    }

    uint32_t gpus_count;
    vkEnumeratePhysicalDevices(instance, &gpus_count, nullptr);
    phys_devices_list.resize(gpus_count);
    vkEnumeratePhysicalDevices(instance, &gpus_count, phys_devices_list.data());


    return true;
}
