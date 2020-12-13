#include "../../headers/vulkan/VKRenderer.hpp"


Engine::VKRenderer::VKRenderer() {

}

void Engine::VKRenderer::render2D() {

}
void Engine::VKRenderer::render3D(Engine::Camera* cam) {

}
void Engine::VKRenderer::InitShaders() {
	MainPipeline = new ZSVulkanPipeline();
	//MainPipeline->Create()
}