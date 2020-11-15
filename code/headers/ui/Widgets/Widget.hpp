#pragma once

#include "../View.hpp"

namespace Engine {
    class IWidget : public IView {
    
    public:
        bool isHoveredByMouse();
        bool isClicked();

        IWidget(unsigned int Width, unsigned int Height);
        IWidget();
        virtual ~IWidget();
    };
}
