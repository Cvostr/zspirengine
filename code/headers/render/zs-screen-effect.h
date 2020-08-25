#ifndef ZS_SCR_EFFECT_HPP
#define ZS_SCR_EFFECT_HPP

namespace Engine {
	class ScreenEffect {
	private:
		void bindFramebuffer();
	public:

		ScreenEffect();
		~ScreenEffect();
	};
}

#endif