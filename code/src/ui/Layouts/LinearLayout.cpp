#include "../../../headers/ui/Layouts/LinearLayout.hpp"

using namespace Engine;

void LinearLayout::AddView(IView* View) {
	mViews.push_back(View);
	View->mParent = this;
}
void LinearLayout::RemoveView(IView* View) {
	std::remove(mViews.begin(), mViews.end(), View);
	mViews.pop_back();
}
