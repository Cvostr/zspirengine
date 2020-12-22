#pragma once

#include <SDL2/SDL.h>
#include "../engine_types.h"
#include "EngineComponentManager.hpp"

namespace Engine {

    typedef struct ZSWINDOW_CREATE_INFO {
        char* title; //title for window
        int Width;
        int Height;
        bool resizable;

        int PosX;
        int PosY;

        ZSWINDOW_CREATE_INFO() {
            Width = 640;
            Height = 480;
            resizable = true;

            PosX = SDL_WINDOWPOS_CENTERED;
            PosY = SDL_WINDOWPOS_CENTERED;
        }
    }ZSWINDOW_CREATE_INFO;

    class Window {
    private:
        SDL_Window* mWindow;
        SDL_GLContext mGLContext;
        Engine::ZSWINDOW_CREATE_INFO* window_info;
        ZSENGINE_CREATE_INFO* engine_info;
        EngineComponentManager* mComponentManager;
    public:

        SDL_Window* GetWindowSDL() {
            return mWindow;
        }

        void SetComponentManager(EngineComponentManager* Manager) { this->mComponentManager = Manager; }
        unsigned int GetWindowWidth() { return window_info->Width; }
        unsigned int GetWindowHeight() { return window_info->Height; }
        void SetTitle(const char* Title);
        void SetSize(unsigned int Width, unsigned int Height);
        void SetWindowMode(unsigned int mode);
        void SetResizeable(bool resizeable);
        void SetGLSwapInterval(int interval);
        void CreateWindow(Engine::ZSWINDOW_CREATE_INFO* win_info, ZSENGINE_CREATE_INFO* engine_info);
        void DestroyWindow();
        void SwapGL();
        static void bindAngelScript(void* mgr);

        Window():
            mWindow(nullptr),
            mComponentManager(nullptr)
        {}
    };
}