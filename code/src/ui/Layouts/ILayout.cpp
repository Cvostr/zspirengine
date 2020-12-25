#include "../../../headers/ui/Layouts/ILayout.hpp"
#include <algorithm>

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

void ILayout::AddView(IView* View) {
	mViews.push_back(View);
	View->mParent = this;
}
void ILayout::RemoveView(IView* View) {
	std::remove(mViews.begin(), mViews.end(), View);
	mViews.pop_back();
}

bool ILayout::IsFirstView(IView* View) {
	return mViews[0] == View;
}