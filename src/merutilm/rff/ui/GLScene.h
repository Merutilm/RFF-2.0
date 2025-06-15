//
// Created by Merutilm on 2025-05-08.
//

#pragma once
#include <windows.h>
#include <string>

namespace merutilm::rff {
    class GLScene {

        const std::string INIT_NOT_CALLED = "Failed to load context. is called initGL()?\n";
        bool init = false;


    public:

        HWND renderWindow = nullptr;
        HDC hdc = nullptr;
        HGLRC context = nullptr;

        virtual ~GLScene() = default;
        virtual void configure(HWND wnd, HDC hdc, HGLRC context);
        virtual void renderGL() = 0;
        void makeContextCurrent() const;
        void swapBuffers() const;


        HWND getRenderWindow() const;

        HDC getHDC() const;

        HGLRC getContext() const;

    };
}