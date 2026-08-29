#pragma once

#include "RndComputeShader.hpp"
#include "RndPixelRenderPriority.hpp"

namespace merutilm::rff2 {
    struct RenderSettings {
        float clarityMultiplier;
        float fps;
        RndPixelRenderPriority pixelRenderPriority;
        RndComputeShader computeShader;
    };
}

