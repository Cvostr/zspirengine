#pragma once

#include <vulkan/vulkan.hpp>
#include "../render/Renderer.hpp"
#include "../render/Material.hpp"
#include "VKMaterial.hpp"
#include "ZSVulkanPipeline.hpp"
#include "ZSVulkanRenderPass.hpp"
#include "ZSVulkanFramebuffer.hpp"
#include <vector>
#include <string>
#include "VKUniformBuffer.hpp"

namespace Engine {


	class VKObjectToRender {
	public:
		Engine::GameObject* obj;
		Mat4 transform;
		VKMaterial* mat;

		VKObjectToRender() {
			mat = nullptr;
		}
	};

	class VKRenderer : public Renderer{
	private:
		VkSemaphore imageAvailableSemaphore;
		VkSemaphore ShadowFinishedSemaphore;
		VkSemaphore MaterialsFinishedSemaphore;
		VkSemaphore DefferedFinishedSemaphore;

		VkCommandBuffer m3dCmdBuf;
		VkCommandBuffer mDefferedCmdBuf;
		VkCommandBuffer mShadowCmdBuf;

		VkCommandPool commandPool;

		ZSVulkanPipeline* DefferedPipeline;
		ZSVulkanPipeline* ShadowPipeline;

		ZSVulkanFramebuffer* MaterialFb;
		ZSVulkanRenderPass* MaterialRenderPass;

		ZSVulkanFramebuffer* OutFb;
		ZSVulkanRenderPass* OutRenderPass;

		ZSVulkanFramebuffer* ShadowFb;
		ZSVulkanRenderPass* ShadowRenderPass;

		ZSVulkanSampler* mMaterialSampler;

		std::vector<VKObjectToRender> ObjectsToRender;

		void Present();

		void FillShadowCmdBuf();
		void FillDefferedCmdBuf();
		void Fill3dCmdBuf();
		
	public:
		void SetViewport(VkCommandBuffer cmdbuf, unsigned int startX, unsigned int startY, unsigned int width, unsigned int height);
		void InitShaders();
		void render2D();
		void render3D(Engine::Camera* cam);
		void DrawObject(Engine::GameObject* obj);

		VKRenderer();
	};
}