#pragma once

#include "ILayout.hpp"

namespace Engine {

	enum LayoutOrientation {
		ORIENTATION_HORIZONTAL,
		ORIENTATION_VERTICAL
	};

	class LinearLayout : public ILayout {
	public:
		LayoutOrientation Orientation;
		void AddView(IView* View);
		void RemoveView(IView* View);
		void draw();
		LinearLayout() : Orientation(ORIENTATION_HORIZONTAL) {
			mType = LAYOUT_TYPE_LINEAR;
		}
	};
}