#include <ui/Widgets/Button.hpp>
#include <input/Input.hpp>

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

    if (TextContent != nullptr) {
        Vec2 Pos = Vec2(
            static_cast<float>(pvPos.posX), 
            static_cast<float>(pvPos.posY)
        );
        c->DrawString(this->TextContent->STR[0].c_str(), this->TextContent->STR[0].size(), Pos, TextColor);
    }
}