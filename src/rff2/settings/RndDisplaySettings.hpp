//
// Created by Merutilm on 9/4/26.
//

#pragma once
#include "RndPixelRenderPriority.hpp"

namespace merutilm::rff2 {
    struct RndDisplaySettings {
        float clarityMultiplier;
        float fps;
        RndPixelRenderPriority pixelRenderPriority;
    };
}