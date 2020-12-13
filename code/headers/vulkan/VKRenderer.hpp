#pragma once

#include <vulkan/vulkan.hpp>
#include "../render/Renderer.hpp"
#include "../vulkan/ZSVulkanPipeline.hpp"

namespace Engine {
	class VKRenderer : public Renderer{
	private:
		VkSemaphore imageAvailableSemaphore;
		VkSemaphore renderFinishedSemaphore;

		VkCommandBuffer mCmdBuf;

		ZSVulkanPipeline* MainPipeline;

	public:
		void InitShaders();
		void render2D();
		void render3D(Engine::Camera* cam);

		VKRenderer();
	};
}