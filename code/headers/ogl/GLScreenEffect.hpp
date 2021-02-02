#pragma once

#include "../render/ScreenEffect.hpp"

namespace Engine {
	class GLScreenEffect : public ScreenEffect {
	public:
		void Compute();

		GLScreenEffect(unsigned int width, unsigned int height, TextureFormat Format) :
			ScreenEffect(width, height, Format)
		{
			
		}

		GLScreenEffect() :
			ScreenEffect()
		{

		}
	};
}