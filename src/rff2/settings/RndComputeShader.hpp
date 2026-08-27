//
// Created by Merutilm on 8/27/26.
//

#pragma once
#include "RndCmpMPAMode.hpp"
#include <cstdint>
namespace merutilm::rff2 {

    struct RndComputeShader {
        bool use;
        RndCmpMPAMode mpaMode;
        float preferredBatchDuration;
        uint32_t allowedGlitchPixelCount;
        bool interpolateIsolated;
    };
}