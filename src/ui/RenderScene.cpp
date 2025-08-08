//
// Created by Merutilm on 2025-08-08.
//

#include "RenderScene.hpp"

namespace merutilm::rff2 {
    RenderScene::RenderScene(const HWND window) : window(window) {
        RenderScene::init();
    }

    RenderScene::~RenderScene() {
        RenderScene::destroy();
    }

    void RenderScene::init() {

    }

    void RenderScene::destroy() {

    }

}