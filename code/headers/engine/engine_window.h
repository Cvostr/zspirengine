#ifndef ENGINE_WINDOW_H
#define ENGINE_WINDOW_H

typedef struct ZSWINDOW_CREATE_INFO{
    char* title; //title for window
    int Width;
    int Height;
    bool resizable;

    ZSWINDOW_CREATE_INFO(){
        Width = 640;
        Height = 480;
        resizable = true;
    }
}ZSWINDOW_CREATE_INFO;

#endif // ENGINE_WINDOW_H
