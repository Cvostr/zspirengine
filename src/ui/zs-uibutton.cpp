#include "../../headers/ui/zs-uibutton.h"

extern ZSGAME_DATA* game_data;

ZSUI::Button::Button(){
    this->common_sprite = nullptr;
    this->hovered_sprite = nullptr;
}
ZSUI::Button::~Button(){

}
void ZSUI::Button::draw(){
    if(Widget::isHoveredByMouse())
        game_data->pipeline->renderSprite(this->common_sprite, this->pos.posX, this->pos.posY, size.WIDTH, size.HEIGHT);
    else
        game_data->pipeline->renderSprite(hovered_sprite, this->pos.posX, this->pos.posY, size.WIDTH, size.HEIGHT);
}

bool ZSUI::Button::isClicked(){

}

void ZSUI::Button::resize(unsigned int Width, unsigned int Height){
    Widget::resize(Width, Height);
}
void ZSUI::Button::move(unsigned int x, unsigned int y){
    Widget::move(x, y);
}
