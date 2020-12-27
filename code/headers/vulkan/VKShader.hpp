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

        bool readBinaryShaderFile(std::string path, char** result, size_t& size);

        bool compileFromFile(std::string VSpath, std::string FSpath, std::string GSpath = "");

        void Use();
        void Destroy();

        vkShader();
        ~vkShader();
    };

}