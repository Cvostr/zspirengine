#pragma once

#include "Math.hpp"
#include "GpuObject.hpp"
#include <vulkan/vulkan.hpp>
#include "../engine.h"

#define MAX_LIGHTS_AMOUNT 150

namespace Engine {

	class Shader : public GpuObject {
	public:

        virtual bool compileFromFile(std::string VSpath, std::string FSpath, std::string GSpath = "") { return false; }
        virtual bool compileFromStr(const char* _VS, const char* _FS, const char* _GS = nullptr) { return false; }
        virtual void setUniformBufferBinding(const char* UB_NAME, unsigned int binding){}
        virtual void Use(){}
        virtual void Destroy(){}

        Shader() { //Construct to set isCreated var to false
        
        }
        virtual ~Shader() {

        }
	};

    Engine::Shader* allocShader();
}
