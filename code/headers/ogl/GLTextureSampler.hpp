#pragma once

#include "../render/TextureSampler.hpp"

namespace Engine {
	class glTextureSampler : public TextureSampler {
	private:
		unsigned int mGLSampler;
	public:

		void CreateSampler();
		void Destroy();

		glTextureSampler() {
			TextureSampler();
			mGLSampler = 0;
		}
	};
}