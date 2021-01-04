#pragma once

#include "../../game.h"
#include "Widget.hpp"


namespace Engine {

    typedef struct ButtonStyle {
        TextureResource* mCommonSprite;
        TextureResource* mHoveredSprite;

        RGBAColor mTextColor;
        RGBAColor mTextColorHovered;

        ViewSize size;

        ButtonStyle() : 
            mCommonSprite(nullptr),
            mHoveredSprite(nullptr)
        {
            size.WIDTH = 100;
            size.HEIGHT = 50;
        }

    }ButtonStyle;

    class Button : public IWidget{
    private:
        

        TextureResource* mCommonSprite;
        TextureResource* mHoveredSprite;
    public:
        void SetDefaultSprite(TextureResource* sprite) { mCommonSprite = sprite; }
        void SetHoveredSprite(TextureResource* sprite) { mHoveredSprite = sprite; }

        void draw();

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
