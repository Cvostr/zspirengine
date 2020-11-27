#pragma once

#include <vulkan/vulkan.hpp>
#include "ZSVulkanDevice.hpp"

namespace Engine {

    typedef struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    }SwapChainSupportDetails;

    class ZSVulkanSwapChain {
    private:
        VkSwapchainKHR mSwapChain;
        SwapChainSupportDetails SW_Details;

        std::vector<VkImage> mSwapChainImages;
        std::vector<VkImageView> mSwapChainImageViews;

        VkExtent2D swap_extend;

        void CreateImages(ZSVulkanDevice* Device, VkSurfaceFormatKHR ChosenSurfaceFormat);

    public:
        bool initSwapchain(ZSVulkanDevice* Device, ZSVulkanInstance* instance, int Width, int Height);

        VkSwapchainKHR GetSwapChain() { return mSwapChain; }

        ZSVulkanSwapChain();
    };

    //ZSVulkanDevice* CreatePrimaryDevice(ZSVulkanInstance* instance);

}