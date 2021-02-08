#include "../../headers/render/TextureSampler.hpp"

#include "../../headers/ogl/GLTextureSampler.hpp"
#include "../../headers/vulkan/ZSVulkanSampler.hpp"

#include "../../headers/game.h"

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