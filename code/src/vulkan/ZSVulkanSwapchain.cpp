#include <vulkan/ZSVulkanSwapchain.hpp>
#include <engine/Logger.hpp>

#include <game.h>

extern ZSGAME_DATA* game_data;

VkImageView Engine::ZSVulkanSwapChain::GetImageViewAtIndex(unsigned int Index) {
    if (Index > this->mSwapChainImageViews.size())
        Index = static_cast<uint32_t>(mSwapChainImageViews.size());
    return mSwapChainImageViews[Index];
}

bool Engine::ZSVulkanSwapChain::initSwapchain(ZSVulkanDevice* Device, ZSVulkanInstance* instance, int Width, int Height) {
    SwapChainSupportDetails result;
    //Get surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Device->getPhysicalDevice(), instance->GetSurface(), &result.capabilities);

    uint32_t surface_format_count = 0;
    //Get surface formats
    vkGetPhysicalDeviceSurfaceFormatsKHR(Device->getPhysicalDevice(), instance->GetSurface(), &surface_format_count, nullptr);

    result.formats.resize(surface_format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(Device->getPhysicalDevice(), instance->GetSurface(), &surface_format_count, result.formats.data());

    uint32_t present_mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(Device->getPhysicalDevice(), instance->GetSurface(), &present_mode_count, nullptr);

    result.presentModes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(Device->getPhysicalDevice(), instance->GetSurface(), &present_mode_count, result.presentModes.data());


    for (unsigned int i = 0; i < result.formats.size(); i++) {
        VkSurfaceFormatKHR format = result.formats[i];
        if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && format.format == VK_FORMAT_B8G8R8A8_UNORM) {
            chosenSurfaceFormat = format;
        }
    }
    //find best presentation mode
    for (unsigned int i = 0; i < result.presentModes.size(); i++) {
        VkPresentModeKHR pres_mode = result.presentModes[i];
        if (pres_mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
            chosenPresentMode = pres_mode;
    }

    swap_extend.width = static_cast<unsigned int>(Width);
    swap_extend.height = static_cast<unsigned int>(Height);

    //Now fill the strcucture
    VkSwapchainCreateInfoKHR swc_create_info;
    swc_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swc_create_info.pNext = nullptr;
    swc_create_info.flags = 0;
    swc_create_info.surface = instance->GetSurface();
    swc_create_info.minImageCount = result.capabilities.minImageCount + 1;
    swc_create_info.imageExtent = swap_extend;
    //Configure image props
    swc_create_info.imageFormat = chosenSurfaceFormat.format;
    swc_create_info.imageColorSpace = chosenSurfaceFormat.colorSpace;

    swc_create_info.imageArrayLayers = 1;
    swc_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; //we'll render directly to images

    swc_create_info.queueFamilyIndexCount = 2;
    uint32_t queueFamilyIndices[] = { Device->GetGraphicsQueueFamilyIndex(), Device->GetPresentQueueFamilyIndex() };
    swc_create_info.pQueueFamilyIndices = queueFamilyIndices;

    if (Device->GetGraphicsQueueFamilyIndex() != Device->GetPresentQueueFamilyIndex()) { //if indices are not equal
        swc_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        //TODO - implement
    }
    else {
        swc_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swc_create_info.queueFamilyIndexCount = 0; // Optional
        swc_create_info.pQueueFamilyIndices = nullptr; // Optional
    }
    //No extra transformations
    swc_create_info.preTransform = result.capabilities.currentTransform;
    //No composite alpha
    swc_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    swc_create_info.presentMode = chosenPresentMode;
    swc_create_info.clipped = VK_TRUE;
    swc_create_info.oldSwapchain = VK_NULL_HANDLE;

    Logger::Log(LogType::LOG_TYPE_INFO) << "Creating Vulkan Swapchain" << "\n";
    //Creating swapchain
    if (vkCreateSwapchainKHR(Device->getVkDevice(), &swc_create_info, nullptr, &this->mSwapChain) != VK_SUCCESS) {
        return false;
    }
    Logger::Log(LogType::LOG_TYPE_INFO) << "Vulkan: Swapchain creation successful" << "\n";

    CreateImages(Device, chosenSurfaceFormat);

    mCreated = true;

    return true;
}

void Engine::ZSVulkanSwapChain::Destroy() {
    VkDevice device = game_data->vk_main->mDevice->getVkDevice();
    if (mCreated) {
        //Destroy created image views
        for (uint32_t img_i = 0; img_i < GetSwapChainImagesCount(); img_i++) {
            vkDestroyImageView(device, mSwapChainImageViews[img_i], nullptr);
        }
        //Clear image view array
        mSwapChainImageViews.clear();
        mSwapChainImages.clear();
        //Destroy Swapchain
        vkDestroySwapchainKHR(device, mSwapChain, nullptr);

        mCreated = false;
    }
}

void Engine::ZSVulkanSwapChain::CreateImages(ZSVulkanDevice* Device, VkSurfaceFormatKHR ChosenSurfaceFormat) {
    uint32_t swc_images;

    vkGetSwapchainImagesKHR(Device->getVkDevice(), this->mSwapChain, &swc_images, nullptr);
    mSwapChainImages.resize(swc_images);
    vkGetSwapchainImagesKHR(Device->getVkDevice(), this->mSwapChain, &swc_images, mSwapChainImages.data());

    this->mSwapChainImageViews.resize(swc_images);
    //Iterate over all swapchain images and create image views
    for (unsigned int sw_i = 0; sw_i < swc_images; sw_i++) {
        VkImageViewCreateInfo img_view_create_info;
        img_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        img_view_create_info.pNext = nullptr;
        img_view_create_info.flags = 0;
        img_view_create_info.image = mSwapChainImages[sw_i];
        img_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        img_view_create_info.format = ChosenSurfaceFormat.format;

        img_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        img_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        img_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        img_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        img_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        img_view_create_info.subresourceRange.baseMipLevel = 0;
        img_view_create_info.subresourceRange.levelCount = 1;
        img_view_create_info.subresourceRange.baseArrayLayer = 0;
        img_view_create_info.subresourceRange.layerCount = 1;

        vkCreateImageView(Device->getVkDevice(), &img_view_create_info, nullptr, &mSwapChainImageViews[sw_i]);
    }
}

Engine::ZSVulkanSwapChain::ZSVulkanSwapChain() :
    mSwapChain(VK_NULL_HANDLE)
{

}