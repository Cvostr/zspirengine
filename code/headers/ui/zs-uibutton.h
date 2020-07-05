#ifndef ZSUIBUTTON_H
#define ZSUIBUTTON_H

#include "../../headers/game.h"
#include "zs-uiwidget.h"

namespace ZSUI {

    typedef struct ButtonStyle {
        Engine::TextureResource* default_sprite;
        Engine::TextureResource* hovered_sprite;

        ZSVECTOR3 text_color;
        ZSVECTOR3 text_color_hovered;

        WidgetSize size;

        ButtonStyle() {
            default_sprite = nullptr;
            hovered_sprite = nullptr;

            size.WIDTH = 100;
            size.HEIGHT = 50;
        }

    }ButtonStyle;

    class Button : public Widget{
    public:
        Engine::TextureResource* common_sprite;
        Engine::TextureResource* hovered_sprite;

        void resize(unsigned int Width, unsigned int Height);
        void move(unsigned int x, unsigned int y);
        bool isClicked();
        void draw();

        Button(ButtonStyle* style);
        Button(unsigned int Width, unsigned int Height);
        Button();
        ~Button();
    };
}

#endif // ZSUIBUTTON_H
