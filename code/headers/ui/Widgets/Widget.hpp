#pragma once

#include "../View.hpp"
#include "../../engine/LocaleStringsFile.h"

namespace Engine {
    class IWidget : public IView {
    protected:
        ZSPIRE::LocString* TextContent;
    public:
        bool isHoveredByMouse();
        bool isClicked();

        void setText(std::string StrId);

        IWidget(unsigned int Width, unsigned int Height);
        IWidget();
        virtual ~IWidget();
    };
}
