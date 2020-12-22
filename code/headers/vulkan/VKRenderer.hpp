#pragma once

#include <vulkan/vulkan.hpp>
#include "../render/Renderer.hpp"
#include "ZSVulkanPipeline.hpp"
#include "ZSVulkanFramebuffer.hpp"
#include <vector>
#include "vk_data.h"

namespace Engine {

	class VKObjectToRender {
	public:
		Engine::GameObject* obj;
		Mat4 transform;
		//Engine::MeshResource* mesh;
	};

	class VKRenderer : public Renderer{
	private:
		VkSemaphore imageAvailableSemaphore;
		VkSemaphore renderFinishedSemaphore;

		VkCommandBuffer mCmdBuf;
		VkCommandPool commandPool;

		ZSVulkanPipeline* MainPipeline;
		ZSVulkanFramebuffer* TestFb;

		_vk_Shader* test_shader;
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