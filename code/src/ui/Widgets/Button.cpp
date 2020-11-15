#include "../../../headers/ui/Widgets/Button.hpp"
#include "../../../headers/input/zs-input.h"

extern ZSGAME_DATA* game_data;

Engine::Button::Button(unsigned int Width, unsigned int Height) {
    this->common_sprite = nullptr;
    this->hovered_sprite = nullptr;
    //Call resize function
    resize(Width, Height);
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
}
bool Engine::Button::isClicked() {
    Input::MouseState* mstate = Input::getMouseStatePtr();
    if (IWidget::isHoveredByMouse() || mstate->isLButtonDown) {
        return true;
    }
    else
        return false;
}

void Engine::Button::resize(unsigned int Width, unsigned int Height){
    IWidget::resize(Width, Height);
}
void Engine::Button::move(unsigned int x, unsigned int y){
    IWidget::move(x, y);
}

void Engine::Button::setText(ZSPIRE::LocString* btn_text_string) {
    this->btn_text_string = btn_text_string;
}