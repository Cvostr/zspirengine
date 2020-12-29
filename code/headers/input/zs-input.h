#pragma once

#include <SDL2/SDL.h>

namespace Input {
    void addPressedKeyToQueue(int keycode);
    void addHeldKeyToQueue(int keycode);
    void removeHeldKeyFromQueue(int keycode);
    void clearMouseState();

    void clearPressedKeys();
    bool isKeyPressed(int keycode);
    bool isKeyHold(int keycode);
    void SetMouseRelativeMode(bool Mode);

    struct MouseState{
        //Current X coordinate of mouse cursor in screen
        int mouseX;
        //Current movement on X of mouse cursor
        int mouseRelX;
        //Current Y coordinate of mouse cursor in screen
        int mouseY;
        //Current movement on Y of mouse cursor
        int mouseRelY;

        bool RelativeMode;

        int mouseWheelX;
        int mouseWheelY;

        //Is Left mouse button pressed
        bool isLButtonDown;
        //Is Right mouse button pressed
        bool isRButtonDown;
        //Is middle mouse button pressed
        bool isMidBtnDown;
    };

    MouseState* getMouseStatePtr();
    MouseState getMouseState();

    void processEventsSDL(SDL_Event* event);
}