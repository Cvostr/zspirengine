#ifndef ZSUIBUTTON_H
#define ZSUIBUTTON_H

#include "../../headers/game.h"
#include "zs-uiwidget.h"

namespace ZSUI {
    class Button : public Widget{
    public:
        Engine::TextureResource* common_sprite;
        Engine::TextureResource* hovered_sprite;

        void resize(unsigned int Width, unsigned int Height);
        void move(unsigned int x, unsigned int y);

        bool isClicked();

        void draw();

        Button();
        ~Button();
    };
}

#endif // ZSUIBUTTON_H
