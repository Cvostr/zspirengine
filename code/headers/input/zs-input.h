#ifndef zs_input
#define zs_input

#include <SDL2/SDL.h>

namespace Input {
    void addPressedKeyToQueue(int keycode);
    void addHeldKeyToQueue(int keycode);
    void removeHeldKeyFromQueue(int keycode);
    void clearMouseState();

    void clearPressedKeys();
    bool isKeyPressed(int keycode);
    bool isKeyHold(int keycode);

    struct MouseState{
        int mouseX;
        int mouseRelX;

        int mouseY;
        int mouseRelY;

        bool isLButtonDown;
        bool isRButtonDown;
        bool isMidBtnDown;
    };

    MouseState* getMouseStatePtr();
    MouseState getMouseState();

    void processEventsSDL(SDL_Event* event);
}

#endif
