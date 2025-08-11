//
// Created by Merutilm on 2025-05-08.
//

#pragma once
#include <windows.h>
#include <string>

namespace merutilm::rff2 {
    class WGLScene {

        const std::string INIT_NOT_CALLED = "Failed to load context. is called initGL()?\n";
        bool init = false;

        HWND renderWindow = nullptr;
        HDC hdc = nullptr;
        HGLRC context = nullptr;

    public:

        virtual ~WGLScene() = default;
        virtual void configure(HWND wnd, HDC hdc, HGLRC context);
        virtual void renderGL() = 0;
        void makeContextCurrent() const;
        void swapBuffers() const;


        [[nodiscard]] HWND getRenderWindow() const;

        [[nodiscard]] HDC getHDC() const;

        [[nodiscard]] HGLRC getContext() const;

    };
}