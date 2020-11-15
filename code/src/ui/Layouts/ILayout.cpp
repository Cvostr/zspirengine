#include "../../../headers/ui/Layouts/ILayout.hpp"

using namespace Engine;

IView* ILayout::GetPreviousObject(IView* View) {
	if (mViews.size() == 1 || mViews[0] == View) return nullptr;
	tViewList::iterator it = mViews.begin();
	for (; it != mViews.end(); it++) {
		if (*it == View)
			return *(it - 1);
	}
	return nullptr;
}
void ILayout::draw() {
	tViewList::iterator it = mViews.begin();

	for (; it != mViews.end(); it++) {
		IView* View = *it;
		View->draw();
	}
}