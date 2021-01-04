#pragma once

#include "../View.hpp"
#include "../../engine/LocaleStringsFile.h"
#include "../../math/RGBColor.hpp"

namespace Engine {
    class IWidget : public IView {
    protected:
        ZSPIRE::LocString* TextContent;

        RGBAColor mTextColor;
        RGBAColor mTextColorHovered;
    public:
        bool isHoveredByMouse();
        bool isClicked();

        void setText(std::string StrId);
        void setTextColor(RGBAColor TextColor) { mTextColor = TextColor; }

        IWidget(unsigned int Width, unsigned int Height);
        IWidget();
        virtual ~IWidget();
    };
}
