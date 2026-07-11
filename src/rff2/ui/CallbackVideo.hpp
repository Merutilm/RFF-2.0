//
// Created by Merutilm on 2025-06-08.
//

#pragma once
#include <functional>

#include "RFFApplication.hpp"

namespace merutilm::rff2 {
    struct CallbackVideo {
        static std::function<void()> fnDataSettings(RFFApplication &arm){return []{};}
        static std::function<void()> fnAnimationSettings(RFFApplication &arm){return []{};}
        static std::function<void()> fnExportSettings(RFFApplication &arm){return []{};}
        static std::function<void()> fnGenerateVidKeyframes(RFFApplication &arm){return []{};}
        static std::function<void()> fnExportZoomVideo(RFFApplication &arm){return []{};}
    };
}
