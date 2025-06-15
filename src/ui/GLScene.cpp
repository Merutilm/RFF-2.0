//
// Created by Merutilm on 2025-05-08.
//
#include "GLScene.h"

#include <iostream>

#include "glad_wgl.h"
#include "Constants.h"
#include <assert.h>


void merutilm::rff::GLScene::configure(const HWND wnd, const HDC hdc, const HGLRC context) {
    this->renderWindow = wnd;
    this->hdc = hdc;
    this->context = context;
    init = true;
}


void merutilm::rff::GLScene::makeContextCurrent() const {
    if (!init) {
        std::cout << INIT_NOT_CALLED << std::flush;
        return;
    }
    assert(wglMakeCurrent(hdc, context));
}


void merutilm::rff::GLScene::swapBuffers() const {
    if (!init) {
        std::cout << INIT_NOT_CALLED << std::flush;
        return;
    }
    wglSwapIntervalEXT(true);
    wglSwapLayerBuffers(hdc, WGL_SWAP_MAIN_PLANE);
}


HWND merutilm::rff::GLScene::getRenderWindow() const {
    return renderWindow;
}

HDC merutilm::rff::GLScene::getHDC() const {
    return hdc;
}

HGLRC merutilm::rff::GLScene::getContext() const {
    return context;
}
