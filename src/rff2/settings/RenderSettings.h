#pragma once

#include "RndComputeShader.hpp"
#include "RndDisplaySettings.hpp"

namespace merutilm::rff2 {
    struct RenderSettings {
        RndDisplaySettings display;
        RndComputeShader computeShader;
    };
}

