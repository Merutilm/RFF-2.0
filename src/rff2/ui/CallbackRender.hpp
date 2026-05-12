//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include <functional>
#include "AppRenderManager.hpp"

namespace merutilm::rff2 {
    struct CallbackRender {
        static std::function<void()> fnSetRenderProperties(AppRenderManager &arm);
        static std::function<bool()> fnGetterLinearInterpolation(AppRenderManager &arm);
        static std::function<void(bool)> fnLinearInterpolation(AppRenderManager &arm);
    };
}
