#pragma once

#include "../engine/EngineComponent.h"
#include "View.hpp"
#include "Layouts/ILayout.hpp"
#include <vector>

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

	private:
		tViewList mViews;
		ILayout* mRootLayout;
	};
}