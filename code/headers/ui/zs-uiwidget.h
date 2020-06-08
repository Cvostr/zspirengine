#ifndef zsuiwidget
#define zsuiwidget

typedef struct WidgetSize{
    unsigned int WIDTH;
    unsigned int HEIGHT;

    WidgetSize(){
        WIDTH = 0;
        HEIGHT = 0;
    }
}WidgetSize;

typedef struct WidgetPosition{
    unsigned int posX;
    unsigned int posY;

    WidgetPosition(){
        posX = 100;
        posY = 100;
    }
}WidgetPosition;

namespace ZSUI {
    class Widget{
    protected:
        WidgetSize size;
        WidgetPosition pos;
    public:
        virtual void resize(unsigned int Width, unsigned int Height);
        virtual void move(unsigned int x, unsigned int y);
        bool isHoveredByMouse();
        bool isClicked();

        virtual void draw();

        Widget();
        virtual ~Widget();
    };
}

#endif
