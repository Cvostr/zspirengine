#pragma once

#include <vector>

#define SIZE_WRAP_CONTENT 0
#define SIZE_FILL_PARENT 0xFFFFFFFF

#define POS_CENTER_PARENT 0xFFFFFFFF

namespace Engine {

    typedef struct ViewSize {
        unsigned int WIDTH;
        unsigned int HEIGHT;

        ViewSize() : WIDTH(0), HEIGHT(0) {}
        ViewSize(unsigned int Width, unsigned int Height) : WIDTH(Width), HEIGHT(Height) {}
    }ViewSize;

    typedef struct ViewMargin {
        unsigned int marginTop;
        unsigned int marginBottom;
        unsigned int marginLeft;
        unsigned int marginRight;

        ViewMargin() : marginTop(10), marginBottom(10), marginLeft(10), marginRight(10) {}
    }ViewMargin;

    typedef struct ViewPosition {
        unsigned int posX;
        unsigned int posY;

        ViewPosition() : posX(0), posY(0) {}
        ViewPosition(unsigned int PosX, unsigned int PosY) : posX(PosX), posY(PosY) {}
    }ViewPosition;

	class IView {
	public:

        virtual void resize(unsigned int Width, unsigned int Height);
        virtual void move(unsigned int x, unsigned int y);

        virtual void resize(const ViewSize& Size);
        virtual void move(const ViewPosition& Pos);

        void setMarginTop(unsigned int marginTop) { margin.marginTop = marginTop; }
        void setMarginBottom(unsigned int marginBottom) { margin.marginBottom = marginBottom; }
        void setMarginLeft(unsigned int marginLeft) { margin.marginLeft = marginLeft; }
        void setMarginRight(unsigned int marginRight) { margin.marginRight = marginRight; }

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