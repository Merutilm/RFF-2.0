//
// Created by Merutilm on 2025-05-08.
//
#include "Win32GLScene.h"

#include <iostream>

#include "glad_wgl.h"
#include "Constants.h"
#include <assert.h>


namespace merutilm::rff {


    void Win32GLScene::configure(const HWND wnd, const HDC hdc, const HGLRC context) {
        this->renderWindow = wnd;
        this->hdc = hdc;
        this->context = context;
        init = true;
    }


    void Win32GLScene::makeContextCurrent() const {
        if (!init) {
            std::cout << INIT_NOT_CALLED << std::flush;
            return;
        }
        assert(wglMakeCurrent(hdc, context));
    }


    void Win32GLScene::swapBuffers() const {
        if (!init) {
            std::cout << INIT_NOT_CALLED << std::flush;
            return;
        }
        wglSwapIntervalEXT(true);
        wglSwapLayerBuffers(hdc, WGL_SWAP_MAIN_PLANE);
    }


    HWND Win32GLScene::getRenderWindow() const {
        return renderWindow;
    }

    HDC Win32GLScene::getHDC() const {
        return hdc;
    }

    HGLRC Win32GLScene::getContext() const {
        return context;
    }
}