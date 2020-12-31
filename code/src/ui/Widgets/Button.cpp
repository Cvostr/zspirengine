#include "../../../headers/ui/Widgets/Button.hpp"
#include "../../../headers/input/zs-input.h"

extern ZSGAME_DATA* game_data;

Engine::Button::Button(unsigned int Width, unsigned int Height) {
    this->common_sprite = nullptr;
    this->hovered_sprite = nullptr;
    //Call resize function
    //resize(Width, Height);
}

Engine::Button::Button(ButtonStyle* style) {
    SetStyle(style);
}

Engine::Button::~Button(){

}

void Engine::Button::SetStyle(ButtonStyle* style) {
    common_sprite = style->default_sprite;
    hovered_sprite = style->hovered_sprite;
    //Call resize function
    resize(style->size.WIDTH, style->size.HEIGHT);
}

void Engine::Button::draw(){
    ViewSize pvSize;
    ViewPosition pvPos;
    __GetTransform(pvSize, pvPos);

    if(IWidget::isHoveredByMouse())
        game_data->pipeline->renderSprite(hovered_sprite, pvPos.posX, pvPos.posY, pvSize.WIDTH, pvSize.HEIGHT);
    else
        game_data->pipeline->renderSprite(common_sprite, pvPos.posX, pvPos.posY, pvSize.WIDTH, pvSize.HEIGHT);
    
    GlyphFontContainer* c = game_data->resources->getFontByLabel("LiberationMono-Regular")->font_ptr;

    if(TextContent != nullptr)
        c->DrawString(this->TextContent->STR[0].c_str(), this->TextContent->STR[0].size(), Vec2(pvPos.posX, pvPos.posY));
}