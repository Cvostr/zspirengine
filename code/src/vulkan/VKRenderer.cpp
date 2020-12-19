#include "../../headers/vulkan/VKRenderer.hpp"
#include "../../headers/game.h"

extern ZSGAME_DATA* game_data;

Engine::VKRenderer::VKRenderer() {
	InitShaders();
}

void Engine::VKRenderer::render2D() {

}
void Engine::VKRenderer::render3D(Engine::Camera* cam) {
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    vkBeginCommandBuffer(mCmdBuf, &beginInfo);

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = MainPipeline->GetRenderPass();
    renderPassInfo.framebuffer = TestFb->GetFramebuffer();

    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = game_data->vk_main->mSwapChain->GetExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearValues[1].depthStencil = { 1.0f, 0 };
    renderPassInfo.clearValueCount = 2;
    renderPassInfo.pClearValues = &clearValues[0];

    vkCmdBeginRenderPass(mCmdBuf, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(mCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, MainPipeline->GetPipeline());

    vkCmdBindDescriptorSets(mCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, MainPipeline->GetPipelineLayout(), 0, MainPipeline->GetDescriptorSetsCount(), MainPipeline->GetDescriptorsSets(), 0, nullptr);

    ((Engine::_vk_Mesh*)Engine::getPlaneMesh2D())->Draw(mCmdBuf);

    vkCmdEndRenderPass(mCmdBuf);
    vkEndCommandBuffer(mCmdBuf);

    Present();
}
void Engine::VKRenderer::InitShaders() {

	test_shader = new Engine::_vk_Shader;
	test_shader->compileFromFile("Shaders/vulkan_test/vert.spv", "Shaders/vulkan_test/frag.spv");

	ZsVkPipelineConf Conf;
    Conf.DescrSetLayout->pushUniformBuffer((Engine::_vk_UniformBuffer*)this->transformBuffer, VK_SHADER_STAGE_VERTEX_BIT);

	MainPipeline = new ZSVulkanPipeline();
	MainPipeline->PushColorOutputAttachment();
	MainPipeline->Create(test_shader, Conf);

	TestFb = new ZSVulkanFramebuffer;
	TestFb->PushOutputAttachment();
    TestFb->Create(MainPipeline);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	vkCreateSemaphore(game_data->vk_main->mDevice->getVkDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphore);
	vkCreateSemaphore(game_data->vk_main->mDevice->getVkDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphore);


    

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = game_data->vk_main->mDevice->GetGraphicsQueueFamilyIndex();
    poolInfo.flags = 0; // Optional

    vkCreateCommandPool(game_data->vk_main->mDevice->getVkDevice(), &poolInfo, nullptr, &commandPool);

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)1;

    vkAllocateCommandBuffers(game_data->vk_main->mDevice->getVkDevice(), &allocInfo, &mCmdBuf);


    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional
}

void Engine::VKRenderer::Present() {
    uint32_t _imageIndex;
    vkAcquireNextImageKHR(game_data->vk_main->mDevice->getVkDevice(), game_data->vk_main->mSwapChain->GetSwapChain(), UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &_imageIndex);
    _imageIndex = 0;


    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &mCmdBuf;

    VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkQueueSubmit(game_data->vk_main->mDevice->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);


    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { game_data->vk_main->mSwapChain->GetSwapChain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    uint32_t imageIndex = 0;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional
    vkQueuePresentKHR(game_data->vk_main->mDevice->GetPresentQueue(), &presentInfo);
}