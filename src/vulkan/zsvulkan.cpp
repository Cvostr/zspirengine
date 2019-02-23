#include "../../headers/vulkan/zsvulkan.h"
#include <iostream>

ZsVulkan::ZsVulkan(){
 //Nothing much to do
}

void ZsVulkan::init(const char* app_name, int app_ver){
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

    std::cout << "Creating Vulkan Instance";
    vkCreateInstance(&vk_inst_create_info, nullptr, &this->instance);

}
