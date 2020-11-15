#include "../../../headers/ui/Widgets/Widget.hpp"
#include "../../../headers/input/zs-input.h"

#include "../../../headers/engine.h"

extern ZSpireEngine* engine_ptr;

Engine::IWidget::IWidget(){

}
Engine::IWidget::IWidget(unsigned int Width, unsigned int Height) {
    resize(Width, Height);
}
Engine::IWidget::~IWidget(){

}
bool Engine::IWidget::isHoveredByMouse(){
    Input::MouseState mstate = Input::getMouseState();
    mstate.mouseY = engine_ptr->window_info->Height - mstate.mouseY;

    ViewSize Size;
    ViewPosition Pos;
    __GetTransform(Size, Pos);

    if(Pos.posX < mstate.mouseX && Pos.posY < mstate.mouseY && Pos.posX + Size.WIDTH > mstate.mouseX && Pos.posY + Size.HEIGHT > mstate.mouseY)
        return true;

    return false;
}
bool Engine::IWidget::isClicked(){
    return (Input::getMouseState().isLButtonDown && isHoveredByMouse());
}
