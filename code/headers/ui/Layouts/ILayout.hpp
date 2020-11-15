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
		virtual void AddView(IView* View){}
		virtual void RemoveView(IView* View) {}
		IView* GetPreviousObject(IView* View){
			if (mViews.size() == 1 || mViews[0] == View) return nullptr;
			tViewList::iterator it = mViews.begin();
			for (; it != mViews.end(); it++) {
				if (*it == View)
					return *(it - 1);
			}
			return nullptr;
		}
		ILayout() : mType(LAYOUT_TYPE_NONE) {}
	protected:
		tViewList mViews;
	};
}