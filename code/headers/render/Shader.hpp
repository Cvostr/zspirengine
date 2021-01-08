#pragma once

#include "Math.hpp"
#include "GpuObject.hpp"
#include <vulkan/vulkan.hpp>
#include "../engine.h"

#define MAX_LIGHTS_AMOUNT 150

#define HAS_VERT_SHADER 0x1
#define HAS_FRAG_SHADER 0x2
#define HAS_GEOM_SHADER 0x4
#define HAS_COMP_SHADER 0x8

typedef unsigned int ShaderStages;

namespace Engine {

	class Shader : public GpuObject {
	public:

        ShaderStages mStages;

        virtual bool compileFromFile(std::string VSpath, std::string FSpath, std::string GSpath = "") { return false; }
        virtual bool compileFromStr(const char* _VS, const char* _FS, const char* _GS = nullptr) { return false; }
        virtual bool compileComputeFromFile(std::string CSpath) { return false; }
        virtual bool compileComputeFromStr(const char* _CS) { return false; }
        virtual void setUniformBufferBinding(const char* UB_NAME, unsigned int binding){}
        virtual void Use(){}
        virtual void Destroy(){}

        Shader() {
        
        }
        virtual ~Shader() {

        }
	};

    Engine::Shader* allocShader();
}
