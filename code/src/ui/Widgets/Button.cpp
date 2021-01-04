#include "../../../headers/ui/Widgets/Button.hpp"
#include "../../../headers/input/zs-input.h"

extern ZSGAME_DATA* game_data;

Engine::Button::Button(unsigned int Width, unsigned int Height) :
    mCommonSprite(nullptr),
    mHoveredSprite(nullptr)
{
    //Call resize function
    resize(Width, Height);
}

Engine::Button::Button(ButtonStyle* style) {
    SetStyle(style);
}

Engine::Button::~Button(){

}

void Engine::Button::SetStyle(ButtonStyle* style) {
    mCommonSprite = style->mCommonSprite;
    mHoveredSprite = style->mHoveredSprite;

    mTextColor = style->mTextColor;
    mTextColorHovered = style->mTextColorHovered;
    //Call resize function
    resize(style->size.WIDTH, style->size.HEIGHT);
}

void Engine::Button::draw(){
    RGBAColor TextColor;
    ViewSize pvSize;
    ViewPosition pvPos;
    __GetTransform(pvSize, pvPos);

    if (IWidget::isHoveredByMouse()) {
        game_data->pipeline->renderSprite(mHoveredSprite, pvPos.posX, pvPos.posY, pvSize.WIDTH, pvSize.HEIGHT);
        TextColor = this->mTextColorHovered;
    }
    else {
        game_data->pipeline->renderSprite(mCommonSprite, pvPos.posX, pvPos.posY, pvSize.WIDTH, pvSize.HEIGHT);
        TextColor = this->mTextColor;
    }
    GlyphFontContainer* c = game_data->resources->getFontByLabel("LiberationMono-Regular")->font_ptr;

    if(TextContent != nullptr)
        c->DrawString(this->TextContent->STR[0].c_str(), this->TextContent->STR[0].size(), Vec2(pvPos.posX, pvPos.posY), TextColor);
}