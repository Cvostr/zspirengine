#ifndef zs_shader
#define zs_shader

#include "zs-math.h"

#include <vulkan/vulkan.hpp>
#include "../engine.h"

#define SHADER_STR_LEN 16384

namespace Engine {

	class Shader {
	protected:
        bool isCreated;
	public:

        virtual bool compileFromFile(std::string VSpath, std::string FSpath);
        virtual bool compileFromStr(const char* _VS, const char* _FS);
        virtual void setUniformBufferBinding(const char* UB_NAME, unsigned int binding);
        virtual void Use();
        virtual void Destroy();

        Shader(); //Construct to set isCreated var to false
        virtual ~Shader();
	};

    Engine::Shader* allocShader();

}


#endif
