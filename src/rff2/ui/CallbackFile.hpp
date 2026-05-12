//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include <functional>

#include "AppRenderManager.hpp"

namespace merutilm::rff2 {
    struct CallbackFile {
        static std::function<void()> fnSaveMap(AppRenderManager &arm);
        static std::function<void()> fnSaveImage(AppRenderManager &arm);
        static std::function<void()> fnSaveLocation(AppRenderManager &arm);
        static std::function<void()> fnLoadMap(AppRenderManager &arm);
        static std::function<void()> fnLoadLocation(AppRenderManager &arm);
    };
}
