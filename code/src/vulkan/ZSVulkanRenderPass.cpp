#include "../../headers/vulkan/ZSVulkanRenderPass.hpp"
#include "../../headers/vulkan/ZSVulkanFramebuffer.hpp"
#include "../../headers/game.h"

using namespace Engine;

extern ZSGAME_DATA* game_data;

VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(game_data->vk_main->mDevice->getPhysicalDevice(), format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}
VkFormat findDepthFormat() {
    return findSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

void ZSVulkanRenderPass::PushColorAttachment(VkFormat Format, VkImageLayout Layout) {
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = Format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = Layout;
    //Push new attachment description
    mAttachmentDescriptions.push_back(colorAttachment);

    //Create Attachment Reference for Depth Attachment
    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment =
        static_cast<uint32_t>(mAttachmentReferences.size());
    colorAttachmentRef.layout = Layout;
    //Push new attachment reference
    mAttachmentReferences.push_back(colorAttachmentRef);
}
void ZSVulkanRenderPass::PushColorOutputAttachment() {
    PushColorAttachment(game_data->vk_main->mSwapChain->GetChosenSurfaceFormat().format, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
}

void ZSVulkanRenderPass::PushDepthAttachment() {
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = VK_FORMAT_D32_SFLOAT;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    //Push new Attachment Description
    mAttachmentDescriptions.push_back(colorAttachment);

    //Create Attachment Reference for Depth Attachment
    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = mAttachmentDescriptions.size() - 1;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    //Push new attachment reference
    DepthDescriptionRef = colorAttachmentRef;
   // mAttachmentReferences.push_back(colorAttachmentRef);
    //return 0; //Return index of attachment in array
}

bool ZSVulkanRenderPass::Create() {
    //Create SubPass
    //This collects attachment references
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount =
        static_cast<uint32_t>(mAttachmentReferences.size());
    subpass.pColorAttachments = mAttachmentReferences.data();
    subpass.pDepthStencilAttachment = &DepthDescriptionRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    //dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    //dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    //dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    //RENDER PASS
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount =
        static_cast<uint32_t>(mAttachmentDescriptions.size());
    renderPassInfo.pAttachments = mAttachmentDescriptions.data();
    //Set subpass
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    //Set dependencies
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(game_data->vk_main->mDevice->getVkDevice(), &renderPassInfo, nullptr, &mRenderPass) != VK_SUCCESS) {
        return false;
    }
}

void ZSVulkanRenderPass::CmdBegin(VkCommandBuffer cmdbuf, ZSVulkanFramebuffer* framebuffer) {
    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = GetRenderPass();
    renderPassInfo.framebuffer = framebuffer->GetFramebuffer();

    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = game_data->vk_main->mSwapChain->GetExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearValues[1].depthStencil = { 1.0f, 0 };
    renderPassInfo.clearValueCount = 2;
    renderPassInfo.pClearValues = &clearValues[0];

    vkCmdBeginRenderPass(cmdbuf, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}