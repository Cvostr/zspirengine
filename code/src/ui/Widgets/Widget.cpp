#include "../../../headers/ui/Widgets/Widget.hpp"
#include "../../../headers/input/Input.hpp"
#include "../../../headers/game.h"
#include "../../../headers/engine.h"

extern ZSpireEngine* engine_ptr;
extern ZSGAME_DATA* game_data;

Engine::IWidget::IWidget():
    TextContent(nullptr) {}
Engine::IWidget::IWidget(unsigned int Width, unsigned int Height) :
    TextContent(nullptr)
{
    resize(Width, Height);
}
Engine::IWidget::~IWidget(){

}
bool Engine::IWidget::isHoveredByMouse(){
    Input::MouseState mstate = Input::getMouseState();

    int uiMouseX = mstate.mouseX;
    int uiMouseY = engine_ptr->GetWindow()->GetWindowHeight() - mstate.mouseY;

    ViewSize Size;
    ViewPosition Pos;
    __GetTransform(Size, Pos);

    if(Pos.posX < uiMouseX && Pos.posY < uiMouseY && (Pos.posX + Size.WIDTH > uiMouseX) && (Pos.posY + Size.HEIGHT > uiMouseY))
        return true;

    return false;
}
bool Engine::IWidget::isClicked(){
    return (Input::getMouseState().isLButtonDown && isHoveredByMouse());
}

void Engine::IWidget::setText(std::string StrId) {
    TextContent = game_data->resources->getStringByStrId(StrId);
}