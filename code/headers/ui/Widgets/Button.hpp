#pragma once

#include "../../game.h"
#include "../../engine/LocaleStringsFile.h"
#include "Widget.hpp"
#include "../../math/Vec3.hpp"

namespace Engine {

    typedef struct ButtonStyle {
        TextureResource* default_sprite;
        TextureResource* hovered_sprite;

        Vec3 text_color;
        Vec3 text_color_hovered;

        ViewSize size;

        ButtonStyle() : default_sprite(nullptr),
                        hovered_sprite(nullptr)
        {
            size.WIDTH = 100;
            size.HEIGHT = 50;
        }

    }ButtonStyle;

    class Button : public IWidget{
    private:
        ZSPIRE::LocString* btn_text_string;

        TextureResource* common_sprite;
        TextureResource* hovered_sprite;
    public:
        void SetDefaultSprite(TextureResource* sprite) { common_sprite = sprite; }
        void SetHoveredSprite(TextureResource* sprite) { hovered_sprite = sprite; }

        void resize(unsigned int Width, unsigned int Height);
        void move(unsigned int x, unsigned int y);
        void draw();

        void setText(ZSPIRE::LocString* btn_text_string);
        void SetStyle(ButtonStyle* style);
        
        Button(ButtonStyle* style);
        Button(unsigned int Width, unsigned int Height);
        Button() {
            ButtonStyle* btnstyle = new ButtonStyle;
            SetStyle(btnstyle);
            delete btnstyle;
        }
        ~Button();
    };
}
