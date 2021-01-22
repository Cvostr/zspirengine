#pragma once

#include "../render/Shader.hpp"

namespace Engine {

    class glShader : public Engine::Shader {
    private:
        unsigned int mShaderID;
    public:
        void GLcheckCompileErrors(unsigned int shader, const char* type, const char* filepath = nullptr);
        bool readShaderFile(const char* path, char** result, size_t& size);

        bool compileFromFile(std::string VSpath, std::string FSpath = "",
                             std::string GSpath = "",
                             std::string TCSpath = "",
                             std::string TESpath = "");
        bool compileFromStr(const char* _VS, const char* _FS,
                            const char* _GS = nullptr, 
                            const char* _TCS = nullptr,
                            const char* _TES = nullptr);

        bool compileComputeFromFile(std::string CSpath);
        bool compileComputeFromStr(const char* _CS);

        void Use();
        void Destroy();

        glShader();
        ~glShader();
    };
}