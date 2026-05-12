//
// Created by Merutilm on 2025-06-08.
//

#pragma once
#include <functional>

#include "AppRenderManager.hpp"

namespace merutilm::rff2 {
    struct CallbackVideo {
        static std::function<void()> fnDataSettings(AppRenderManager &arm);
        static std::function<void()> fnAnimationSettings(AppRenderManager &arm);
        static std::function<void()> fnExportSettings(AppRenderManager &arm);
        static std::function<void()> fnGenerateVidKeyframes(AppRenderManager &arm);
        static std::function<void()> fnExportZoomVideo(AppRenderManager &arm);
    };
}
