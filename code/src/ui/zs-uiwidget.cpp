#include "../../headers/ui/zs-uiwidget.h"
#include "../../headers/input/zs-input.h"

ZSUI::Widget::Widget(){

}
ZSUI::Widget::~Widget(){

}

void ZSUI::Widget::resize(unsigned int Width, unsigned int Height){
    this->size.WIDTH = Width;
    this->size.HEIGHT = Height;

}
void ZSUI::Widget::move(unsigned int x, unsigned int y){
    this->pos.posX = x;
    this->pos.posY = y;
}
bool ZSUI::Widget::isHoveredByMouse(){
    Input::MouseState mstate = Input::getMouseState();

    if(pos.posX < mstate.mouseX && pos.posY < mstate.mouseY && pos.posX + size.WIDTH > mstate.mouseX && pos.posY + size.HEIGHT > mstate.mouseY)
        return true;

    return false;
}
bool ZSUI::Widget::isClicked(){
    return (Input::getMouseState().isLButtonDown && isHoveredByMouse());
}
void ZSUI::Widget::draw(){

}
