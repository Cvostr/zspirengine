#ifndef zsvulkan
#define zsvulkan

#include "vulkan/vulkan.hpp"

#include <vector>

class ZsVulkan{
private:
    VkInstance instance;
    VkPhysicalDevice used_gpu;

    std::vector<VkExtensionProperties> inst_extensions;
    std::vector<VkPhysicalDevice> phys_devices_list;
    std::vector<VkPhysicalDeviceProperties> phys_devices_props;

    VkPhysicalDevice selected_device;
    VkDevice logicalDevice;
    VkQueue graphicsQueue; //queue of logical device
public:
    bool init(const char* app_name, int app_ver);
    void initDevice();
    ZsVulkan();
};

#endif
