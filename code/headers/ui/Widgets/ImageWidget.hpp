#pragma once

#include "../../game.h"
#include "Widget.hpp"


namespace Engine {


    class ImageView : public IWidget {
    private:

        TextureResource* mSprite;
    public:
        void SetSprite(TextureResource* sprite) { mSprite = sprite; }

        void draw();


        ImageView() :
            mSprite(nullptr)
        {
            
        }
        ~ImageView(){}
    };
}