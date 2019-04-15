#include "../../headers/vulkan/zsvulkan.h"
#include <iostream>

ZsVulkan::ZsVulkan(){
 //Nothing much to do
}

bool ZsVulkan::init(const char* app_name, int app_ver, SDL_Window* window){
    this->window_ptr = window; //assign window pointer

    unsigned int ext_count;
    if (!SDL_Vulkan_GetInstanceExtensions(window, &ext_count, nullptr)) return false;

    std::vector<const char*> extensions = {
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME // Sample additional extension
    };
    size_t additional_extension_count = extensions.size();
    extensions.resize(additional_extension_count + ext_count);

    if (!SDL_Vulkan_GetInstanceExtensions(window, &ext_count, extensions.data() + additional_extension_count)) return false;

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
    vk_inst_create_info.enabledExtensionCount = extensions.size();
    vk_inst_create_info.ppEnabledExtensionNames = extensions.data();

    std::cout << "Creating Vulkan Instance" << std::endl;
    if(vkCreateInstance(&vk_inst_create_info, nullptr, &this->instance) != VK_SUCCESS){
        std::cout << "Can't create Vulkan Instance. Terminating" << std::endl;
        return false;
    }

    uint32_t gpus_count;
    vkEnumeratePhysicalDevices(instance, &gpus_count, nullptr);
    //resize vectors
    phys_devices_list.resize(gpus_count);
    phys_devices_props.resize(gpus_count);
    vkEnumeratePhysicalDevices(instance, &gpus_count, phys_devices_list.data());
    //Get properties of all gpu
    for(unsigned int gpu_i = 0; gpu_i < gpus_count; gpu_i ++){
        VkPhysicalDevice device = this->phys_devices_list[gpu_i];
        vkGetPhysicalDeviceProperties(device, &this->phys_devices_props[gpu_i]);
    }
    this->selected_device = phys_devices_list[0];

    initDevice();
    initSurface();
    return true;
}

void ZsVulkan::initDevice(){
    std::vector<VkQueueFamilyProperties> qFamilyProps;
    uint32_t qFamilyPropCount;

    vkGetPhysicalDeviceQueueFamilyProperties(selected_device, &qFamilyPropCount, nullptr);
    qFamilyProps.resize(qFamilyPropCount);
    vkGetPhysicalDeviceQueueFamilyProperties(selected_device, &qFamilyPropCount, &qFamilyProps[0]);

    int family_index = -1;
    int present_family_index = -1;

    VkDeviceQueueCreateInfo qCreateInfo = {};
    //VkDeviceCreateInfo

    for(unsigned int q_i = 0; q_i < qFamilyPropCount; q_i ++){
        VkQueueFamilyProperties prop = qFamilyProps[q_i];
        if((prop.queueFlags & VK_QUEUE_GRAPHICS_BIT) && family_index < 0)
            family_index = q_i;
        bool canPresent = false;
        if(vkGetPhysicalDeviceSurfaceSupportKHR(device, q_i, surface, &canPresent)){
            present_family_index = q_i;
        }
    }
    if(family_index >= 0){ //if we found right queue family
        qCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        qCreateInfo.pNext = nullptr;
        qCreateInfo.queueFamilyIndex = static_cast<uint32_t>(family_index);
        qCreateInfo.queueCount = 1;
        qCreateInfo.flags = 0;
        float priority = 1.0f;
        qCreateInfo.pQueuePriorities = &priority;
    }

    VkDeviceCreateInfo logical_gpu_create_info = {};
    logical_gpu_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    logical_gpu_create_info.pNext = nullptr;
    logical_gpu_create_info.flags = 0;
    logical_gpu_create_info.queueCreateInfoCount = 1; //1 queue
    logical_gpu_create_info.pQueueCreateInfos = &qCreateInfo;
    //create logical device
    vkCreateDevice(selected_device, &logical_gpu_create_info, nullptr, &logicalDevice);
    //get graphics queue
    vkGetDeviceQueue(logicalDevice, static_cast<uint32_t>(family_index), 0, &this->graphicsQueue);

}

void ZsVulkan::initSurface(){
    if(!SDL_Vulkan_CreateSurface(window_ptr, instance, &this->vk_surface)){
        std::cout << "Can't create Vulkan Window Surface. Terminating..." << std::endl;
    }
}
