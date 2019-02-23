#ifndef zsvulkan
#define zsvulkan

#include "vulkan/vulkan.hpp"

class ZsVulkan{
private:
    VkInstance instance;
public:
    void init(const char* app_name, int app_ver);
    ZsVulkan();
};

#endif
