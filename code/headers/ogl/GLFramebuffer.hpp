#pragma once

#include <render/Renderer.hpp>
#include "../ogl/GLTexture.hpp"

namespace Engine {
	class GLframebuffer : public Framebuffer {
	private:
		GLuint mFramebuffer;
	public:
		void bind();
		void bindTextures(unsigned int m);
		//Inherited from Framebuffer
		void AddTexture(TextureFormat Format = TextureFormat::FORMAT_RGBA);
		void AddDepth(unsigned int Layers = 1, TextureFormat Format = FORMAT_DEPTH_24_STENCIL_8);
		
		void AddTexture(uint32_t Width, uint32_t Height, TextureFormat Format);
		void AddDepth(uint32_t Width, uint32_t Height, unsigned int Layers = 1, TextureFormat Format = FORMAT_DEPTH_24_STENCIL_8);
		//Push Texture to framebuffer
		void AddTexture(Texture* Texture);

		void Create();
		void Destroy();
		void SetSize(uint32_t Width, uint32_t Height);
		
		~GLframebuffer();
		GLframebuffer(unsigned int width, unsigned int height);
		GLframebuffer();
	};
}