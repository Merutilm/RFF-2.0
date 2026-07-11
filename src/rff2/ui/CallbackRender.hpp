//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include <functional>
#include "RFFApplication.hpp"

namespace merutilm::rff2 {
    struct CallbackRender {
        static std::function<void()> fnSetRenderProperties(RFFApplication &arm){return []{};}
        static std::function<bool()> fnGetterLinearInterpolation(RFFApplication &arm);
        static std::function<void(bool)> fnLinearInterpolation(RFFApplication &arm);
    };
}
