#include "../../headers/vulkan/zsvk_mem.h"

extern ZsVulkan* vulkan_ptr;

VkPhysicalDeviceMemoryProperties getPhysDeviceMemoryProps(){
    VkPhysicalDeviceMemoryProperties result;
    vkGetPhysicalDeviceMemoryProperties(vulkan_ptr->getPhysicalDevice(), &result);
    return result;
}
