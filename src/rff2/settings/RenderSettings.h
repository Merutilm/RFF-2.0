#pragma once
#include <cstdint>

#include "RndMPAModeForComputeShader.hpp"

namespace merutilm::rff2 {
    struct RenderSettings {
        float clarityMultiplier;
        float fps;
        bool ptbWithComputeShader;
        RndMPAModeForComputeShader mpaModeForComputeShader;
    };
}

