#include <render/Framebuffer.hpp>

#include <ogl/GLFramebuffer.hpp>
#include <vulkan/ZSVulkanFramebuffer.hpp>

extern ZSpireEngine* engine_ptr;

Engine::Framebuffer* Engine::allocFramebuffer() {
    Engine::Framebuffer* result = nullptr;
    switch (engine_ptr->engine_info->graphicsApi) {
    case OGL: {
        result = new GLframebuffer;
        break;
    }
    case VULKAN: {
        result = new ZSVulkanFramebuffer;
        break;
    }
    }
    return result;
}

Engine::Framebuffer* Engine::allocFramebuffer(uint32_t Width, uint32_t Height) {
    Engine::Framebuffer* result = nullptr;
    switch (engine_ptr->engine_info->graphicsApi) {
    case OGL: {
        result = new GLframebuffer(Width, Height);
        break;
    }
    case VULKAN: {
        result = new ZSVulkanFramebuffer(Width, Height);
        break;
    }
    }
    return result;
}