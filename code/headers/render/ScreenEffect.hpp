#pragma once

#include "Shader.hpp"
#include "Texture.h"

namespace Engine {
	class ScreenEffect {
	protected:
		std::vector<Texture*> mInputTextures;
		Texture* OutputTexture;
		Shader* mEffectShader;

		uint32_t mWidth;
		uint32_t mHeight;

		uint32_t mLocalSizeX;
		uint32_t mLocalSizeY;
		uint32_t mLocalSizeZ;
	public:
		void SetLocalSize(uint32_t LocalSizeX, uint32_t LocalSizeY, uint32_t LocalSizeZ = 1){
			mLocalSizeX = LocalSizeX;
			mLocalSizeY = LocalSizeY;
			mLocalSizeZ = LocalSizeZ;
		}
		void SetSize(uint32_t Width, uint32_t Height) {
			mWidth = Width;
			mHeight = Height;
		}
		void PushInputTexture(Texture* Texture) { mInputTextures.push_back(Texture); }
		void ClearInputTextures() { mInputTextures.clear(); }
		Texture* GetOutputTexture() { return OutputTexture; }
		void SetOutputTexture(Texture* Texture) { OutputTexture = Texture; }
		void SetEffectShader(Shader* Shader) { mEffectShader = Shader; }
		void CompileShaderFromFile(const char* File) {
			mEffectShader = allocShader();
			mEffectShader->compileComputeFromFile(File);
		}
		virtual void Compute(){}

		explicit ScreenEffect(unsigned int width, unsigned int height, TextureFormat Format) :
			mEffectShader(nullptr),
			OutputTexture(allocTexture()),
			mWidth(width),
			mHeight(height),

			mLocalSizeX(1),
			mLocalSizeY(1),
			mLocalSizeZ(1)
		{
			OutputTexture->Create(width, height, Format);
		}

		explicit ScreenEffect() :
			mLocalSizeX(1),
			mLocalSizeY(1),
			mLocalSizeZ(1)
		{

		}
	};
}