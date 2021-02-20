#include <vulkan/ZSVulkanDevice.hpp>
#include <engine/Logger.hpp>

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    "VK_EXT_extended_dynamic_state"
};

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

Engine::ZSVulkanDevice* Engine::CreatePrimaryDevice(ZSVulkanInstance* instance) {
    std::vector<VkPhysicalDevice> phys_devices_list;

    uint32_t gpus_count;
    vkEnumeratePhysicalDevices(instance->GetInstance(), &gpus_count, nullptr);
    //resize vectors
    phys_devices_list.resize(gpus_count);
    vkEnumeratePhysicalDevices(instance->GetInstance(), &gpus_count, phys_devices_list.data());
    //Get properties of all gpu
    for (unsigned int gpu_i = 0; gpu_i < gpus_count; gpu_i++) {
        VkPhysicalDevice device = phys_devices_list[gpu_i];
        VkPhysicalDeviceProperties DeviceProps;
        vkGetPhysicalDeviceProperties(device, &DeviceProps);
        
        if (DeviceProps.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            ZSVulkanDevice* ZsDevice = new ZSVulkanDevice;
            Logger::Log(LogType::LOG_TYPE_INFO) << "Creating Vulkan GPU " << DeviceProps.deviceName << "\n";
            ZsDevice->initDevice(device, instance);
            return ZsDevice;
        }
    }
    return nullptr;
}

Engine::ZSVulkanDevice::ZSVulkanDevice() {

}

bool Engine::ZSVulkanDevice::initDevice(VkPhysicalDevice Device, ZSVulkanInstance* instance) {
    mPhysicalDevice = Device;
    uint32_t vkQueueFamilyPropsCount = 0;
    std::vector<VkQueueFamilyProperties> vkQueueFamilyProps;
    //Obtain queue family props
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &vkQueueFamilyPropsCount, nullptr);
    vkQueueFamilyProps.resize(vkQueueFamilyPropsCount);
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &vkQueueFamilyPropsCount, vkQueueFamilyProps.data());

    VkDeviceQueueCreateInfo QueueGraphicsCreateInfo = {};
    VkDeviceQueueCreateInfo QueuePresentCreateInfo = {};
    std::vector<VkDeviceQueueCreateInfo> QueuesToCreate;

    GraphicsQueueFamilyIndex = -1;
    PresentQueueFamilyIndex = -1;

    for (int32_t q_i = 0; q_i < vkQueueFamilyPropsCount; q_i++) {
        VkQueueFamilyProperties prop = vkQueueFamilyProps[q_i];
        if ((prop.queueFlags & VK_QUEUE_GRAPHICS_BIT) && GraphicsQueueFamilyIndex < 0)
            GraphicsQueueFamilyIndex = q_i;
        VkBool32 canPresent = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(Device, q_i, instance->GetSurface(), &canPresent);
        if (canPresent && PresentQueueFamilyIndex < 0) {
            PresentQueueFamilyIndex = q_i;
        }
    }

    if (GraphicsQueueFamilyIndex >= 0 && PresentQueueFamilyIndex >= 0) { //if we found right queue family
        QueueGraphicsCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        QueueGraphicsCreateInfo.pNext = nullptr;
        QueueGraphicsCreateInfo.queueFamilyIndex = static_cast<uint32_t>(GraphicsQueueFamilyIndex);
        QueueGraphicsCreateInfo.queueCount = 1;
        QueueGraphicsCreateInfo.flags = 0;
        float priority = 1.0f;
        QueueGraphicsCreateInfo.pQueuePriorities = &priority;

        QueuePresentCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        QueuePresentCreateInfo.pNext = nullptr;
        QueuePresentCreateInfo.queueFamilyIndex = static_cast<uint32_t>(PresentQueueFamilyIndex);
        QueuePresentCreateInfo.queueCount = 1;
        QueuePresentCreateInfo.flags = 0;
        QueuePresentCreateInfo.pQueuePriorities = &priority;
        //push create structs to vector
        QueuesToCreate.push_back(QueueGraphicsCreateInfo);
        QueuesToCreate.push_back(QueuePresentCreateInfo);
    }

    VkPhysicalDeviceFeatures features = {};
    features.geometryShader = true;

    VkDeviceCreateInfo logical_gpu_create_info = {};
    logical_gpu_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    logical_gpu_create_info.pNext = nullptr;
    logical_gpu_create_info.flags = 0;
    logical_gpu_create_info.queueCreateInfoCount = static_cast<unsigned int>(QueuesToCreate.size()); //size of queues vector
    logical_gpu_create_info.pQueueCreateInfos = QueuesToCreate.data(); //pointer to start of queues vector
    logical_gpu_create_info.enabledExtensionCount = static_cast<unsigned int>(deviceExtensions.size());
    logical_gpu_create_info.ppEnabledExtensionNames = &deviceExtensions[0];
    logical_gpu_create_info.pEnabledFeatures = &features;

    if (true) {
        logical_gpu_create_info.enabledLayerCount = static_cast<unsigned int>(validationLayers.size());
        logical_gpu_create_info.ppEnabledLayerNames = validationLayers.data();
    }

    //create logical device
    if (vkCreateDevice(Device, &logical_gpu_create_info, nullptr, &mDevice) != VK_SUCCESS) //creating logical device
        return false;                                                           
    //get graphics queue
    vkGetDeviceQueue(mDevice, static_cast<uint32_t>(GraphicsQueueFamilyIndex), 0, &this->mGraphicsQueue);
    //get present queue
    vkGetDeviceQueue(mDevice, static_cast<uint32_t>(PresentQueueFamilyIndex), 0, &this->mPresentQueue);

    return true;
}