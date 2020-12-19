#include "../../headers/ui/UiManager.hpp"
#include <algorithm>

using namespace Engine;

UiManager::UiManager() : mRootLayout(nullptr) {

}

void UiManager::AddView(IView* View) {
	mViews.push_back(View);
}
void UiManager::RemoveView(IView* View) {
	std::remove(mViews.begin(), mViews.end(), View);
	mViews.pop_back();
}
void UiManager::SetRootLayout(ILayout* View) {
	mRootLayout = View;
}
ILayout* UiManager::GetRootLayout() {
	return mRootLayout;
}
bool UiManager::HasRootLayout() {
	return mRootLayout != nullptr;
}
void UiManager::DrawRootLayout() {
	if (HasRootLayout())
		mRootLayout->draw();
}

void UiManager::bindAngelScript(Engine::AGScriptMgr* mgr) {
	mgr->RegisterObjectType("Ui", 0, asOBJ_REF | asOBJ_NOCOUNT);
	mgr->RegisterObjectMethod("Ui", "void SetRootLayout(View@)", asMETHOD(UiManager, SetRootLayout), asCALL_THISCALL);
}