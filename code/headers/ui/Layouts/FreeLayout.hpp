#pragma once

#include "ILayout.hpp"

namespace Engine {
	class FreeLayout : public ILayout {
	public:
		void draw();
		FreeLayout() {}
	};
}