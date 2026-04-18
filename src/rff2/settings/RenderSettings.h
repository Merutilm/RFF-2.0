#pragma once
#include <cstdint>

namespace merutilm::rff2 {
    struct RenderSettings {
        float clarityMultiplier;
        float fps;
        bool linearInterpolation;
        uint32_t threads;
    };
}

