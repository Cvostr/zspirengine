#pragma once

#include "../View.hpp"

namespace Engine {

	enum LayoutType {
		LAYOUT_TYPE_NONE,
		LAYOUT_TYPE_LINEAR,
		LAYOUT_TYPE_FREE
	};

	class ILayout : public IView {
	public:
		LayoutType mType;
		void AddView(IView* View);
		void RemoveView(IView* View);
		IView* GetPreviousObject(IView* View);
		void draw();
		ILayout() : mType(LAYOUT_TYPE_NONE) {}
	protected:
		tViewList mViews;
	};
}