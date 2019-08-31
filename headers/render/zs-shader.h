#ifndef zs_shader
#define zs_shader

#include "zs-math.h"

#include <vulkan/vulkan.hpp>
#include "../engine.h"

#include "../world/zs-camera.h"

namespace Engine {
class VkShaderBracket;

	class Shader {
	protected:
		unsigned int SHADER_ID;
        VkShaderBracket* vulkan_shader;
	public:

		void Init();
        bool compileFromFile(std::string VSpath, std::string FSpath, ZSpireEngine* engine);
        void GLcheckCompileErrors(unsigned int shader, const char* type, const char* filepath = nullptr);
        bool readShaderFile(const char* path, char* result);
        bool readBinaryShaderFile(std::string path, char* result, int* size);
		void Use();
		void Destroy();

		void setGLuniformColor(const char* uniform_str, ZSRGBCOLOR value);
		void setGLuniformFloat(const char* uniform_str, float value);
		void setGLuniformInt(const char* uniform_str, int value);
		void setGLuniformVec3(const char* uniform_str, ZSVECTOR3 value);
        void setGLuniformVec4(const char* uniform_str, ZSVECTOR4 value);
		void setGLuniformMat4x4(const char* uniform_str, ZSMATRIX4x4 value);

		void setHasDiffuseTextureProperty(bool hasDiffuseMap);
		void setHasNormalTextureProperty(bool hasNormalMap);
		void setTextureCountProperty(int tX, int tY);
		void setTransform(ZSMATRIX4x4 transform);
        void setCamera(Camera* cam, bool sendPos = false);

        bool isCreated;

        Shader(); //Construct to set isCreated var to false
	};

    class VkShaderBracket{
    public:
        VkShaderModule vertexShader;
        VkShaderModule fragmentShader;

        VkPipelineShaderStageCreateInfo vertStageInfo;
        VkPipelineShaderStageCreateInfo fragStageInfo;
    };
}


#endif
