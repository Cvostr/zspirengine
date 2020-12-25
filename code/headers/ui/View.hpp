#pragma once

#include <vector>

#define SIZE_WRAP_CONTENT 0
#define SIZE_FILL_PARENT 0xFFFFFFFF

#define POS_CENTER_PARENT 0xFFFFFFFF

namespace Engine {

    typedef struct ViewSize {
        int WIDTH;
        int HEIGHT;

        ViewSize() : WIDTH(0), HEIGHT(0) {}
        ViewSize(int Width, int Height) : WIDTH(Width), HEIGHT(Height) {}
    }ViewSize;

    typedef struct ViewMargin {
        int marginTop;
        int marginBottom;
        int marginLeft;
        int marginRight;

        ViewMargin() : marginTop(10), marginBottom(10), marginLeft(10), marginRight(10) {}
        ViewMargin(int Top, int Bottom, int Left, int Right) :
            marginTop(Top), marginBottom(Bottom), marginLeft(Left), marginRight(Right) {}
    }ViewMargin;

    typedef struct ViewPosition {
        int posX;
        int posY;

        ViewPosition() : posX(0), posY(0) {}
        ViewPosition(int PosX, int PosY) : posX(PosX), posY(PosY) {}
    }ViewPosition;

	class IView {
	public:

        void resize(int Width, int Height);
        void move(int x, int y);

        void resize(const ViewSize& Size);
        void move(const ViewPosition& Pos);

        void setMarginTop(int marginTop) { margin.marginTop = marginTop; }
        void setMarginBottom(int marginBottom) { margin.marginBottom = marginBottom; }
        void setMarginLeft(int marginLeft) { margin.marginLeft = marginLeft; }
        void setMarginRight(int marginRight) { margin.marginRight = marginRight; }

        virtual ViewSize& __GetSizeWrapContent() { return size; }
        void __GetTransform(ViewSize& _Size, ViewPosition& _Pos);

        ViewSize& GetSize() { return size; }
        ViewPosition& GetPos() { return pos; }
        ViewMargin& GetMargin() { return margin; }

		IView* mParent;

        IView();
        virtual ~IView();
		virtual void draw(){}

    protected:
        ViewSize size;
        ViewPosition pos;
        ViewMargin margin;
	};

	typedef std::vector<IView*> tViewList;
}