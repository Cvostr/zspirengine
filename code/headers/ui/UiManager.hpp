#pragma once

#include "../engine/EngineComponent.h"
#include "View.hpp"
#include "Layouts/ILayout.hpp"
#include <vector>
#include "../Scripting/AngelScriptMgr.h"

namespace Engine {
	class UiManager : public IEngineComponent {
	public:

		UiManager();

		void AddView(IView* View);
		void RemoveView(IView* View);
		void SetRootLayout(ILayout* View);
		ILayout* GetRootLayout();
		bool HasRootLayout();
		void DrawRootLayout();
		void RemoveAllViews();


		static void bindAngelScript(Engine::AGScriptMgr* mgr);
	private:
		tViewList mViews;
		ILayout* mRootLayout;
	};
}