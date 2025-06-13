//
// Created by Merutilm on 2025-05-08.
//
#include "RFFScene.h"

#include <iostream>

#include "glad_wgl.h"
#include "RFF.h"
#include <assert.h>


void RFFScene::configure(const HWND wnd, const HDC hdc, const HGLRC context) {
    this->renderWindow = wnd;
    this->hdc = hdc;
    this->context = context;
    init = true;
}


void RFFScene::makeContextCurrent() const {
    if (!init) {
        std::cout << INIT_NOT_CALLED << std::flush;
        return;
    }
    assert(wglMakeCurrent(hdc, context));
}


void RFFScene::swapBuffers() const {
    if (!init) {
        std::cout << INIT_NOT_CALLED << std::flush;
        return;
    }
    wglSwapIntervalEXT(true);
    wglSwapLayerBuffers(hdc, WGL_SWAP_MAIN_PLANE);
}


HWND RFFScene::getRenderWindow() const {
    return renderWindow;
}

HDC RFFScene::getHDC() const {
    return hdc;
}

HGLRC RFFScene::getContext() const {
    return context;
}
