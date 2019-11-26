#include "../../headers/vulkan/zsvulkan.h"
#include "../../headers/vulkan/vk_data.h"
#include <iostream>

ZsVulkan* vulkan_ptr = nullptr;

ZsVulkan::ZsVulkan(){
    this->window_ptr = nullptr;
}

VkDevice ZsVulkan::getVkDevice(){
    return this->logicalDevice;
}

VkPhysicalDevice ZsVulkan::getPhysicalDevice(){
    return this->selected_device;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const std::vector<const char*> validationLayers = {
    "VK_LAYER_LUNARG_standard_validation"
};

std::vector<const char*> extensions = {
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
    "VK_EXT_debug_report"
};

bool ZsVulkan::init(bool validate, const char* app_name, int app_ver, SDL_Window* window, ZSWINDOW_CREATE_INFO* win_info){
    vulkan_ptr = this;

    this->window_ptr = window; //assign window pointer

    unsigned int ext_count;
    if (!SDL_Vulkan_GetInstanceExtensions(window, &ext_count, nullptr)) return false;


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
    if(validate){
        vk_inst_create_info.enabledLayerCount = validationLayers.size();
        vk_inst_create_info.ppEnabledLayerNames = validationLayers.data();
    }

    std::cout << "Creating Vulkan Instance" << std::endl;
    if(vkCreateInstance(&vk_inst_create_info, nullptr, &this->instance) != VK_SUCCESS){
        std::cout << "Can't create Vulkan Instance. Terminating" << std::endl;
        return false;
    }

    if(validate){
        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.pNext = nullptr;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr; // Optional

        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");


        func(this->instance, &createInfo, nullptr, &this->debugMessenger);
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

    initDevice(validate);

    initSwapChain(win_info);

    return true;
}

bool ZsVulkan::initDevice(bool validate){
    //Init vulkan surface
    initSurface();

    std::vector<VkQueueFamilyProperties> qFamilyProps;
    uint32_t qFamilyPropCount;

    vkGetPhysicalDeviceQueueFamilyProperties(selected_device, &qFamilyPropCount, nullptr);
    qFamilyProps.resize(qFamilyPropCount);
    vkGetPhysicalDeviceQueueFamilyProperties(selected_device, &qFamilyPropCount, &qFamilyProps[0]);

    int family_index = -1;
    int present_family_index = -1;

    VkDeviceQueueCreateInfo qCreateInfo = {};
    VkDeviceQueueCreateInfo qPresentCreateInfo = {};

    std::vector<VkDeviceQueueCreateInfo> queues_toCreate;

    for(unsigned int q_i = 0; q_i < qFamilyPropCount; q_i ++){
        VkQueueFamilyProperties prop = qFamilyProps[q_i];
        if((prop.queueFlags & VK_QUEUE_GRAPHICS_BIT) && family_index < 0)
            family_index = q_i;
        VkBool32 canPresent = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(selected_device, q_i, vk_surface, &canPresent);
        if(canPresent){
            present_family_index = q_i;
        }
    }
    if(family_index >= 0 && present_family_index >= 0){ //if we found right queue family
        qCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        qCreateInfo.pNext = nullptr;
        qCreateInfo.queueFamilyIndex = static_cast<uint32_t>(family_index);
        qCreateInfo.queueCount = 1;
        qCreateInfo.flags = 0;
        float priority = 1.0f;
        qCreateInfo.pQueuePriorities = &priority;

        qPresentCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        qPresentCreateInfo.pNext = nullptr;
        qPresentCreateInfo.queueFamilyIndex = static_cast<uint32_t>(present_family_index);
        qPresentCreateInfo.queueCount = 1;
        qPresentCreateInfo.flags = 0;
        qPresentCreateInfo.pQueuePriorities = &priority;
        //push create structs to vector
        queues_toCreate.push_back(qCreateInfo);
        queues_toCreate.push_back(qPresentCreateInfo);
    }

    VkDeviceCreateInfo logical_gpu_create_info = {};
    logical_gpu_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    logical_gpu_create_info.pNext = nullptr;
    logical_gpu_create_info.flags = 0;
    logical_gpu_create_info.queueCreateInfoCount = queues_toCreate.size(); //size of queues vector
    logical_gpu_create_info.pQueueCreateInfos = &queues_toCreate[0]; //pointer to start of queues vector
    logical_gpu_create_info.enabledExtensionCount = deviceExtensions.size();
    logical_gpu_create_info.ppEnabledExtensionNames = &deviceExtensions[0];

    if(validate){
        logical_gpu_create_info.enabledLayerCount = validationLayers.size();
        logical_gpu_create_info.ppEnabledLayerNames = validationLayers.data();
    }

    std::cout << "Creating Vulkan GPU " << this->phys_devices_props[0].deviceName << std::endl;
    //create logical device
    vkCreateDevice(selected_device, &logical_gpu_create_info, nullptr, &logicalDevice); //creating logical device
    //get graphics queue
    vkGetDeviceQueue(logicalDevice, static_cast<uint32_t>(family_index), 0, &this->graphicsQueue);
    //get present queue
    vkGetDeviceQueue(logicalDevice, static_cast<uint32_t>(present_family_index), 0, &this->presentQueue);

    q_f_indices.graphicsIndex = static_cast<uint32_t>(family_index);
    q_f_indices.presentIndex = static_cast<uint32_t>(present_family_index);
}

void ZsVulkan::initSurface(){
    std::cout << "Creating Vulkan surface" << std::endl;
    if(!SDL_Vulkan_CreateSurface(window_ptr, instance, &this->vk_surface)){
        std::cout << "Can't create Vulkan Window Surface. Terminating..." << std::endl;
    }
}

SwapChainSupportDetails ZsVulkan::getSwapChainDetails(){
    SwapChainSupportDetails result;
    //Get surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->selected_device, this->vk_surface, &result.capabilities);

    uint32_t surface_format_count = 0;
    //Get surface formats
    vkGetPhysicalDeviceSurfaceFormatsKHR(this->selected_device, this->vk_surface, &surface_format_count, nullptr);

    result.formats.resize(surface_format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(this->selected_device, this->vk_surface, &surface_format_count, result.formats.data());

    uint32_t present_mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(this->selected_device, this->vk_surface, &present_mode_count, nullptr);

    result.presentModes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(this->selected_device, this->vk_surface, &present_mode_count, result.presentModes.data());

    return result;
}

bool ZsVulkan::initSwapChain(ZSWINDOW_CREATE_INFO* win_info){
    SwapChainSupportDetails swc_details = getSwapChainDetails();
    //Find best surface format
    VkSurfaceFormatKHR chosen_sf_format;
    for(unsigned int i = 0; i < swc_details.formats.size(); i ++){
        VkSurfaceFormatKHR format = swc_details.formats[i];
        if(format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && format.format == VK_FORMAT_B8G8R8A8_UNORM){
            chosen_sf_format = format;
        }
    }
    //find best presentation mode
    VkPresentModeKHR chosen_present_mode;
    for(unsigned int i = 0; i < swc_details.presentModes.size(); i ++){
        VkPresentModeKHR pres_mode = swc_details.presentModes[i];
        if(pres_mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
            chosen_present_mode = pres_mode;
    }
    //configure swap extend
    VkExtent2D swap_extend;
    swap_extend.width = win_info->Width;
    swap_extend.height = win_info->Height;

    //Now fill the strcucture
    VkSwapchainCreateInfoKHR swc_create_info;
    swc_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swc_create_info.pNext = nullptr;
    swc_create_info.flags = 0;
    swc_create_info.surface = this->vk_surface;
    swc_create_info.minImageCount = swc_details.capabilities.minImageCount + 1;
    swc_create_info.imageExtent = swap_extend;
    //Configure image props
    swc_create_info.imageFormat = chosen_sf_format.format;
    swc_create_info.imageColorSpace = chosen_sf_format.colorSpace;

    swc_create_info.imageArrayLayers = 1;
    swc_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; //we'll render directly to images

    if(q_f_indices.graphicsIndex != q_f_indices.presentIndex){ //if indices are not equal
        swc_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        //TODO - implement
    }else{
        swc_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    //No extra transformations
    swc_create_info.preTransform = swc_details.capabilities.currentTransform;
    //No comosite alpha
    swc_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    swc_create_info.presentMode = chosen_present_mode;
    swc_create_info.clipped = VK_TRUE;
    swc_create_info.oldSwapchain = VK_NULL_HANDLE;

    std::cout << "Creating Vulkan Swapchain" << std::endl;
    //Creating swapchain
    if(!vkCreateSwapchainKHR(this->logicalDevice, &swc_create_info, nullptr, &this->vk_swapchain)){
        return false;
    }
    //array of swapchain images
    std::vector<VkImage> swapChainImages;
    uint32_t swc_images;

    vkGetSwapchainImagesKHR(this->logicalDevice, this->vk_swapchain, &swc_images, nullptr);
    swapChainImages.resize(swc_images);
    vkGetSwapchainImagesKHR(this->logicalDevice, this->vk_swapchain, &swc_images, swapChainImages.data());

    this->swapChainImageViews.resize(swc_images);
    //Iterate over all swapchain images and create image views
    for(unsigned int sw_i = 0; sw_i < swc_images; sw_i ++){
        VkImageViewCreateInfo img_view_create_info;
        img_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        img_view_create_info.pNext = nullptr;
        img_view_create_info.image = swapChainImages[sw_i];
        img_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        img_view_create_info.format = chosen_sf_format.format;

        img_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        img_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        img_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        img_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        img_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        img_view_create_info.subresourceRange.baseMipLevel = 0;
        img_view_create_info.subresourceRange.levelCount = 1;
        img_view_create_info.subresourceRange.baseArrayLayer = 0;
        img_view_create_info.subresourceRange.layerCount = 1;

        vkCreateImageView(this->logicalDevice, &img_view_create_info, nullptr, &this->swapChainImageViews[sw_i]);
    }
    return true;
}

void ZsVkPipeline::create(void* shader, ZsVkPipelineConf conf, ZsVulkan* vulkan){
    Engine::_vk_Shader* shader_ptr = static_cast<Engine::_vk_Shader*>(shader);
    VkPipelineShaderStageCreateInfo vertexStageCreateInfo = {}, fragmentStageCreateInfo = {};

    vertexStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexStageCreateInfo.flags = 0;
    vertexStageCreateInfo.pNext = nullptr;
    vertexStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexStageCreateInfo.module = shader_ptr->vertexShader;
    vertexStageCreateInfo.pName = "main";

    fragmentStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentStageCreateInfo.flags = 0;
    fragmentStageCreateInfo.pNext = nullptr;
    fragmentStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentStageCreateInfo.module = shader_ptr->fragmentShader;
    fragmentStageCreateInfo.pName = "main";


    //IA state
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = conf.iaTopology;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    //Viewport & Scissors congiration
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &conf.viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &conf.scissor;
    //Rasterizer conf
    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = conf.polygonMode;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = conf.cullFace;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
    //MSAA configuration
    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional
    //Color blend
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    VkPipelineLayoutCreateInfo pipeline_info = {};
    pipeline_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_info.setLayoutCount = 0;
    pipeline_info.pSetLayouts = nullptr;
    pipeline_info.pushConstantRangeCount = 0;
    pipeline_info.pPushConstantRanges = nullptr;
    vkCreatePipelineLayout(vulkan->getVkDevice(), &pipeline_info, nullptr, &this->pipelineLayout);
}

VkPipeline ZsVkPipeline::getPipeline(){
    return this->pipeline;
}
