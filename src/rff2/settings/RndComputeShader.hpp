//
// Created by Merutilm on 8/27/26.
//

#pragma once
#include <cstdint>
namespace merutilm::rff2 {

    struct RndComputeShader {
        bool use;
        float preferredBatchDuration;
        uint32_t allowedGlitchPixelCount;
        bool completelyIgnoreMpa;
        uint32_t automaticAcceptMpaBatches;
        bool interpolateIsolated;
    };
}