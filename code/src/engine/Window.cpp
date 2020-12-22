#include "../../headers/engine/Window.hpp"
#include <iostream>
#include "../../headers/engine/Logger.hpp"
#include "../../headers/Scripting/AngelScriptMgr.h"

using namespace Engine;

void Window::SetTitle(const char* Title) {
    SDL_SetWindowTitle(mWindow, Title);
}

void Window::SetSize(unsigned int Width, unsigned int Height) {
    if (Width < 1 && Height < 1) return;

    if(mWindow)
	    SDL_SetWindowSize(this->mWindow, Width, Height);
    window_info->Width = Width;
    window_info->Height = Height;
    if(mComponentManager)
        mComponentManager->updateResolution(Width, Height);
}

void Window::SetWindowMode(unsigned int mode) {
	SDL_SetWindowFullscreen(this->mWindow, mode);
}

void Window::SetGLSwapInterval(int interval) {
    SDL_GL_SetSwapInterval(interval);
}

void Window::SetResizeable(bool resizeable) {
    SDL_SetWindowResizable(mWindow, SDL_bool(resizeable));
}

void Window::CreateWindow(ZSWINDOW_CREATE_INFO* win_info, ZSENGINE_CREATE_INFO* engine_info) {
    this->window_info = win_info;
    this->engine_info = engine_info;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) //Trying to init SDL
    {
        std::cout << "Error opening window " << SDL_GetError() << std::endl;
    }

    unsigned int SDL_WIN_MODE = 0;
    if (engine_info->graphicsApi == OGL) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        //OpenGL 4.3
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_WIN_MODE = SDL_WINDOW_OPENGL; //Set window mode to OpenGL
        Engine::Logger::Log() << "OpenGL version : 4.3 core\n";
    }
    else if (engine_info->graphicsApi == VULKAN) {
        SDL_WIN_MODE = SDL_WINDOW_VULKAN;
    }

    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    Engine::Logger::Log() << "Opening SDL2 window\n";
    this->mWindow = SDL_CreateWindow(win_info->title, window_info->PosX, window_info->PosY, win_info->Width, win_info->Height, SDL_WIN_MODE); //Create window

    if (engine_info->graphicsApi == OGL)
        this->mGLContext = SDL_GL_CreateContext(mWindow);
}

void Window::SwapGL() {
    SDL_GL_SwapWindow(mWindow); //Send rendered frame
}

void Window::DestroyWindow() {
    SDL_DestroyWindow(mWindow); //Destroy SDL and opengl
    SDL_GL_DeleteContext(mGLContext);
}

void Window::bindAngelScript(void* _mgr) {
    Engine::AGScriptMgr* mgr = (AGScriptMgr*)_mgr;
    mgr->RegisterObjectType("Window", 0, asOBJ_REF | asOBJ_NOCOUNT);
    mgr->RegisterObjectMethod("Window", "void SetWindowSize(uint, uint)", asMETHOD(Window, SetSize), asCALL_THISCALL);
    mgr->RegisterObjectMethod("Window", "void SetWindowMode(uint)", asMETHOD(Window, SetWindowMode), asCALL_THISCALL);
}