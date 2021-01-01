#pragma once

#include <vulkan/vulkan.hpp>
#include "../render/Renderer.hpp"
#include "ZSVulkanPipeline.hpp"
#include "ZSVulkanRenderPass.hpp"
#include "ZSVulkanFramebuffer.hpp"
#include <vector>
#include "VKUniformBuffer.hpp"

namespace Engine {

	class VKObjectToRender {
	public:
		Engine::GameObject* obj;
		Mat4 transform;
		Material* mat;

		VKObjectToRender() {
			mat = nullptr;
		}
	};

	class VKRenderer : public Renderer{
	private:
		VkSemaphore imageAvailableSemaphore;
		VkSemaphore renderFinishedSemaphore;

		VkCommandBuffer mCmdBuf;
		VkCommandPool commandPool;

		ZSVulkanPipeline* MainPipeline;

		ZSVulkanFramebuffer* TestFb;
		ZSVulkanRenderPass* MaterialRenderPass;

		ZSVulkanFramebuffer* OutFb;
		ZSVulkanRenderPass* OutRenderPass;

		ZSVulkanSampler* mMaterialSampler;

		std::vector<VKObjectToRender> ObjectsToRender;

		void Present();

	public:
		void InitShaders();
		void render2D();
		void render3D(Engine::Camera* cam);
		void DrawObject(Engine::GameObject* obj);

		VKRenderer();
	};
}