#include <vulkan/VKRenderer.hpp>
#include <game.h>

extern ZSGAME_DATA* game_data;

void Engine::VKPresentInfrastructure::Create(VkCommandPool Pool, int Images) {
    OutRenderPass = new ZSVulkanRenderPass;
    OutRenderPass->PushColorOutputAttachment();
    OutRenderPass->Create();

    for (unsigned int fb_i = 0; fb_i < Images; fb_i++) {
        ZSVulkanFramebuffer* OutFb = new ZSVulkanFramebuffer;
        OutFb->PushOutputAttachment(fb_i);
        OutFb->Create(OutRenderPass);

        this->OutFramebuffers.push_back(OutFb);
    }

    PresentCmdbufs.resize(Images);

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = Pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)Images;
    vkAllocateCommandBuffers(game_data->vk_main->mDevice->getVkDevice(), &allocInfo, PresentCmdbufs.data());


    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    vkCreateSemaphore(game_data->vk_main->mDevice->getVkDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphore);
    vkCreateSemaphore(game_data->vk_main->mDevice->getVkDevice(), &semaphoreInfo, nullptr, &PresentBeginSemaphore);
}
void Engine::VKPresentInfrastructure::Destroy() {
    for (unsigned int fb_i = 0; fb_i < OutFramebuffers.size(); fb_i++) {
        delete OutFramebuffers[fb_i];
    }

    delete OutRenderPass;

    vkDestroySemaphore(game_data->vk_main->mDevice->getVkDevice(), imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(game_data->vk_main->mDevice->getVkDevice(), PresentBeginSemaphore, nullptr);
}

void Engine::VKPresentInfrastructure::RecreateSwapchain() {
    ZSVulkanSwapChain* swapchain = game_data->vk_main->mSwapChain;
    Window* win = game_data->window;

    swapchain->Destroy();

    swapchain->initSwapchain(game_data->vk_main->mDevice, game_data->vk_main->mInstance,
        win->GetWindowWidth(), win->GetWindowHeight());

    for (unsigned int fb_i = 0; fb_i < OutFramebuffers.size(); fb_i++) {
        delete OutFramebuffers[fb_i];
    }
    OutFramebuffers.clear();

    for (unsigned int fb_i = 0; fb_i < swapchain->GetSwapChainImagesCount(); fb_i++) {
        ZSVulkanFramebuffer* OutFb = new ZSVulkanFramebuffer;
        OutFb->PushOutputAttachment(fb_i);
        OutFb->Create(OutRenderPass);

        this->OutFramebuffers.push_back(OutFb);
    }
}