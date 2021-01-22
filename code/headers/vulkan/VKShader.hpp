#pragma once

#include "../render/Shader.hpp"
#include <vulkan/vulkan.hpp>

namespace Engine {
    class vkShader : public Engine::Shader {
    private:

    public:

        VkShaderModule vertexShader;
        VkShaderModule fragmentShader;
        VkShaderModule geometryShader;
        VkShaderModule tessControlShader;
        VkShaderModule tessEvalShader;

        bool readBinaryShaderFile(std::string path, char** result, size_t& size);

        bool compileFromFile(std::string VSpath,
            std::string FSpath = "",
            std::string GSpath = "",
            std::string TCSpath = "",
            std::string TESpath = "");

        void Use();
        void Destroy();

        vkShader();
        ~vkShader();
    };

}