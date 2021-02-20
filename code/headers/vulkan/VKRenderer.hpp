#pragma once

#include <vulkan/vulkan.hpp>
#include "../render/Renderer.hpp"
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
		int TransformArrayIndex;
		int SkinningArrayIndex;

		Engine::GameObject* obj;
		Mat4 transform;
		VKMaterial* mat;

		VKObjectToRender() {
			mat = nullptr;
			TransformArrayIndex = 0;
			SkinningArrayIndex = 0;
		}
	};

	class VKCameraToRender {
	public:
		void* camera_prop;
		VkCommandBuffer GBufferCmdBuf;
		VkCommandBuffer DefferedCmdBuf;

		int Index;

		VKCameraToRender() {
			camera_prop = nullptr;

			GBufferCmdBuf = nullptr;
			DefferedCmdBuf = nullptr;

			Index = 0;
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

		ZSVulkanRenderPass* GBufferRenderPass;
		ZSVulkanRenderPass* DefferedRenderPass;

		ZSVulkanFramebuffer* OutFb;
		ZSVulkanRenderPass* OutRenderPass;

		ZSVulkanFramebuffer* ShadowFb;
		ZSVulkanRenderPass* ShadowRenderPass;

		ZSVulkanSampler* mMaterialSampler;

		vkUniformBuffer* TransformStorageBuf;
		vkUniformBuffer* SkinningStorageBuf;
		vkUniformBuffer* CamerasStorageBuf;

		std::vector<VKObjectToRender> ObjectsToRender;
		std::vector<VKCameraToRender> CamerasToRender;

		void Present();
		void ComputeAll();

		void FillShadowCmdBuf();
		void FillDefferedCmdBuf();
		void Fill3dCmdBuf();

		int LastTransformOffset;
		int LastSkinningOffset;
		
	public:

		vkUniformBuffer* GetTransformStorageBuffer() { return TransformStorageBuf; }
		vkUniformBuffer* GetSkinningStorageBuffer() { return SkinningStorageBuf; }
		vkUniformBuffer* GetCamerasStorageBuffer() { return CamerasStorageBuf; }

		void SetViewport(VkCommandBuffer cmdbuf, unsigned int startX, unsigned int startY, unsigned int width, unsigned int height);
		void InitShaders();
		void render2D();
		void render3D();
		void DrawObject(Engine::GameObject* obj);

		void Render3DCamera(void* cam_prop, uint32_t cam_index);


		VKRenderer();
	};
}