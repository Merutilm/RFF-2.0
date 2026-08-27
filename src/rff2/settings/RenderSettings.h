#pragma once

#include "RndComputeShader.hpp"

namespace merutilm::rff2 {
    struct RenderSettings {
        float clarityMultiplier;
        float fps;
        RndComputeShader computeShader;
    };
}

