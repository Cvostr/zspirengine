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

    unsigned int uiMouseX = static_cast<unsigned int>(mstate.mouseX);
    unsigned int uiMouseY = static_cast<unsigned int>(mstate.mouseY);

    ViewSize Size;
    ViewPosition Pos;
    __GetTransform(Size, Pos);

    if(Pos.posX < uiMouseX && Pos.posY < uiMouseY && Pos.posX + Size.WIDTH > uiMouseX && Pos.posY + Size.HEIGHT > uiMouseY)
        return true;

    return false;
}
bool Engine::IWidget::isClicked(){
    return (Input::getMouseState().isLButtonDown && isHoveredByMouse());
}
