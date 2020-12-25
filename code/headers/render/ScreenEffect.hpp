#pragma once

#include "Shader.hpp"
#include "Texture.h"

namespace Engine {
	class ScreenEffect {
	private:
		std::vector<Texture*> mInputTextures;
		Texture* OutputTexture;
		Shader* mEffectShader;
	public:
		void PushInputTexture(Texture* Texture) { mInputTextures.push_back(Texture); }
		Texture* GetOutputTexture() { return OutputTexture; }
		void SetEffectShader(Shader* Shader) { mEffectShader = Shader; }
		void CompileShaderFromFile(const char* File) {
			mEffectShader = allocShader();
			mEffectShader->compileComputeFromFile(File);
		}
		virtual void Compute(){}

		ScreenEffect(unsigned int width, unsigned int height, TextureFormat Format) :
			mEffectShader(nullptr),
			OutputTexture(allocTexture())
		{
			OutputTexture->Create(width, height, Format);
		}
	};
}