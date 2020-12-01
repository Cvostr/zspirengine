#pragma once

#include <vulkan/vulkan.hpp>
#include "../render/Renderer.hpp"

namespace Engine {
	class VKRenderer : public Renderer{
	private:
		VkSemaphore imageAvailableSemaphore;
		VkSemaphore renderFinishedSemaphore;

		VkCommandBuffer mCmdBuf;

	public:
		void InitShaders();
		void render2D();
		void render3D(Engine::Camera* cam);

		VKRenderer();
	};
}