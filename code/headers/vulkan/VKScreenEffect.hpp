#pragma once

#include "../render/ScreenEffect.hpp"

namespace Engine {
	class VKScreenEffect : public ScreenEffect {
	private:
		VkPipeline mPipeline;
	public:
		void Compute();

		VKScreenEffect(unsigned int width, unsigned int height, TextureFormat Format) :
			ScreenEffect(width, height, Format)
		{

		}

		VKScreenEffect() :
			ScreenEffect()
		{
			
		}
	};
}