#include <render/TextureSampler.hpp>

#include <ogl/GLTextureSampler.hpp>
#include <vulkan/ZSVulkanSampler.hpp>

#include <game.h>

extern ZSpireEngine* engine_ptr;

Engine::TextureSampler* Engine::allocTextureSampler() {
    Engine::TextureSampler* result = nullptr;
    switch (engine_ptr->engine_info->graphicsApi) {
    case OGL: {
        result = new glTextureSampler;
        break;
    }
    case VULKAN: {
        result = new ZSVulkanSampler;
        break;
    }
    }
    return result;
}