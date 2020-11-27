#pragma once

#include <vulkan/vulkan.hpp>
#include "ZSVulkanInstance.hpp"

namespace Engine {

    class ZSVulkanDevice {
    private:
        VkPhysicalDevice mPhysicalDevice;
        VkDevice mDevice; //logical device, created from physical device

        VkQueue mGraphicsQueue; //queue of logical device
        VkQueue mPresentQueue; //queue to present

        int32_t GraphicsQueueFamilyIndex;
        int32_t PresentQueueFamilyIndex;

    public:
        bool initDevice(VkPhysicalDevice Device, ZSVulkanInstance* instance);

        VkDevice getVkDevice() { return mDevice; }
        VkPhysicalDevice getPhysicalDevice() { return mPhysicalDevice; }

        VkQueue GetGraphicsQueue() { return mGraphicsQueue; }
        VkQueue GetPresentQueue() { return mPresentQueue;  }

        int32_t GetGraphicsQueueFamilyIndex() { return GraphicsQueueFamilyIndex; }
        int32_t GetPresentQueueFamilyIndex() { return PresentQueueFamilyIndex; }

        ZSVulkanDevice();
    };

    ZSVulkanDevice* CreatePrimaryDevice(ZSVulkanInstance* instance);

}