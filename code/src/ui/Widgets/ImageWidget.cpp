#include <ui/Widgets/ImageWidget.hpp>

extern ZSGAME_DATA* game_data;

void Engine::ImageView::draw() {
    ViewSize pvSize;
    ViewPosition pvPos;
    __GetTransform(pvSize, pvPos);

    game_data->pipeline->renderSprite(mSprite, pvPos.posX, pvPos.posY, pvSize.WIDTH, pvSize.HEIGHT);
}