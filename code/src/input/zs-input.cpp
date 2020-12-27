#include "../../headers/input/zs-input.h"

#define KEYS_QUEUE_SIZE 10
#define KEY_NONE -200

static Input::MouseState mouse;

static int pressed_keys_queue[KEYS_QUEUE_SIZE];
static int pressed_keys_q_size = 0;
static int hold_keys_queue[KEYS_QUEUE_SIZE];
static int hold_keys_q_size = 0;

void Input::addPressedKeyToQueue(int keycode){
    //if we have enough free space
    if(pressed_keys_q_size > KEYS_QUEUE_SIZE - 1) return;
    pressed_keys_queue[pressed_keys_q_size] = keycode;
    pressed_keys_q_size += 1;
}
void Input::addHeldKeyToQueue(int keycode){
    //if key already held, do nothing
    if(isKeyHold(keycode)) return;

    bool insertable = false;
    int insert_pos = 0;

    for(int iterator = 0; iterator < hold_keys_q_size; iterator ++){
        if(hold_keys_queue[iterator] == KEY_NONE){
            insertable = true;
            insert_pos = iterator;
        }
    }
    if(!insertable){
        if(hold_keys_q_size > KEYS_QUEUE_SIZE - 1) return;
        hold_keys_queue[hold_keys_q_size] = keycode;
        hold_keys_q_size += 1;
    }else{
        hold_keys_queue[insert_pos] = keycode;
    }
}
void Input::removeHeldKeyFromQueue(int keycode){
    for(int iterator = 0; iterator < hold_keys_q_size; iterator ++){
        if(hold_keys_queue[iterator] == keycode){
            hold_keys_queue[iterator] = KEY_NONE;
        }
    }
}
void Input::clearPressedKeys(){
    pressed_keys_q_size = 0;
}
bool Input::isKeyPressed(int keycode){
    for(int i = 0; i < pressed_keys_q_size; i ++){
        if(pressed_keys_queue[i] == keycode)
            return true;
    }
    return false;
}
bool Input::isKeyHold(int keycode){
    for(int i = 0; i < hold_keys_q_size; i ++){
        if(hold_keys_queue[i] == keycode)
            return true;
    }
    return false;
}

void Input::SetMouseRelativeMode(bool Mode) {
    SDL_SetRelativeMouseMode((SDL_bool)Mode);
}

Input::MouseState* Input::getMouseStatePtr(){
    return &mouse;
}
Input::MouseState Input::getMouseState(){
    return mouse;
}

void Input::clearMouseState(){
    mouse.mouseRelX = 0;
    mouse.mouseRelY = 0;
}

void Input::processEventsSDL(SDL_Event* event){
    if (event->type == SDL_KEYDOWN) { //if user pressed a key on keyboard
        //w.edit_win_ptr->onKeyDown(event.key.keysym); //Call press function on EditWindow
        Input::addPressedKeyToQueue(event->key.keysym.sym);
        Input::addHeldKeyToQueue(event->key.keysym.sym);

    }
    if (event->type == SDL_KEYUP) { //if user pressed a key on keyboard
        Input::removeHeldKeyFromQueue(event->key.keysym.sym);

    }
    if (event->type == SDL_MOUSEMOTION) { //If user moved mouse
        //update state in ZSENSDK
        mouse.mouseX = event->motion.x;
        mouse.mouseY = event->motion.y;
        mouse.mouseRelX = event->motion.xrel;
        mouse.mouseRelY = event->motion.yrel;
    }
    if (event->type == SDL_MOUSEBUTTONUP) { //If user released mouse button

        if (event->button.button == SDL_BUTTON_LEFT) {
            mouse.isLButtonDown = false;
        }
        if (event->button.button == SDL_BUTTON_RIGHT) {
            mouse.isRButtonDown = false;
        }
        if (event->button.button == SDL_BUTTON_MIDDLE) {
            mouse.isMidBtnDown = false;
        }
    }
    if (event->type == SDL_MOUSEBUTTONDOWN) { //If user pressed mouse btn
        if (event->button.button == SDL_BUTTON_LEFT) {
            mouse.isLButtonDown = true;
        }
        if (event->button.button == SDL_BUTTON_RIGHT) {
            mouse.isRButtonDown = true;
        }
        if (event->button.button == SDL_BUTTON_MIDDLE) {
            mouse.isMidBtnDown = true;
        }
    }
    if (event->type == SDL_MOUSEWHEEL) {

    }
}
