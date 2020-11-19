#include "../../headers/ui/View.hpp"

#include "../../headers/ui/Layouts/ILayout.hpp"
#include "../../headers/ui/Layouts/LinearLayout.hpp"

#include "../../headers/game.h"

extern ZSGAME_DATA* game_data;

Engine::IView::IView() : 
	mParent(nullptr)
{
	game_data->ui_manager->AddView(this);
}

Engine::IView::~IView() {
	game_data->ui_manager->RemoveView(this);
}

void Engine::IView::resize(unsigned int Width, unsigned int Height) {
	this->size = ViewSize(Width, Height);
}
void Engine::IView::move(unsigned int x, unsigned int y) {
	this->pos = ViewPosition(x, y);
}
void Engine::IView::resize(const ViewSize& Size) {
	this->size = Size;
}
void Engine::IView::move(const ViewPosition& Pos) {
	this->pos = Pos;
}

void Engine::IView::__GetTransform(ViewSize& _Size, ViewPosition& _Pos) {
	ViewSize WrapContentSize = __GetSizeWrapContent();
	_Size = this->size;
	_Pos = this->pos;
	if (this->size.WIDTH == SIZE_WRAP_CONTENT)
		_Size.WIDTH = WrapContentSize.WIDTH;
	if (this->size.HEIGHT == SIZE_WRAP_CONTENT)
		_Size.HEIGHT = WrapContentSize.HEIGHT;

	ILayout* ParentLayout = static_cast<ILayout*>(mParent);
	if (mParent != nullptr) {

		if (ParentLayout->mType == LAYOUT_TYPE_LINEAR) {
			LinearLayout* _LinearLayout = static_cast<LinearLayout*>(ParentLayout);
			//Get previous object
			IView* PreviousView = static_cast<ILayout*>(mParent)->GetPreviousObject(this);
			ViewSize pvSize;
			ViewPosition pvPos;
			if (PreviousView != nullptr)
				PreviousView->__GetTransform(pvSize, pvPos);
			if (_LinearLayout->Orientation == ORIENTATION_HORIZONTAL) {
				_Pos.posX = pvPos.posX + pvSize.WIDTH + margin.marginLeft;
			}
			if (_LinearLayout->Orientation == ORIENTATION_VERTICAL) {
				_Pos.posX = pvPos.posY + pvSize.HEIGHT + margin.marginTop;
			}
		}
		else {
			_Pos = this->pos;
		}

		ViewSize parentSize;
		ViewPosition parentPos;
		mParent->__GetTransform(parentSize, parentPos);
		_Pos.posX += parentPos.posX;
		_Pos.posY += parentPos.posY;
	}

		
}