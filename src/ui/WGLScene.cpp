//
// Created by Merutilm on 2025-05-08.
//
#include "WGLScene.h"

#include <iostream>

#include "glad_wgl.h"
#include "Constants.h"
#include <assert.h>


namespace merutilm::rff2 {


    void WGLScene::configure(const HWND wnd, const HDC hdc, const HGLRC context) {
        this->renderWindow = wnd;
        this->hdc = hdc;
        this->context = context;
        init = true;
    }


    void WGLScene::makeContextCurrent() const {
        if (!init) {
            std::cout << INIT_NOT_CALLED << std::flush;
            return;
        }
        assert(wglMakeCurrent(hdc, context));
    }


    void WGLScene::swapBuffers() const {
        if (!init) {
            std::cout << INIT_NOT_CALLED << std::flush;
            return;
        }
        wglSwapIntervalEXT(true);
        wglSwapLayerBuffers(hdc, WGL_SWAP_MAIN_PLANE);
    }


    HWND WGLScene::getRenderWindow() const {
        return renderWindow;
    }

    HDC WGLScene::getHDC() const {
        return hdc;
    }

    HGLRC WGLScene::getContext() const {
        return context;
    }
}