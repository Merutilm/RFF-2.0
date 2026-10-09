//
// Created by Merutilm on 2025-05-16.
//

#pragma once
#include "RFF2.hpp"

namespace merutilm::rff2 {
    struct FnShader {
        static void palette(RFF2 &app);
        static void stripe(RFF2 &app);
        static void surface(RFF2 &app);
        static void color(RFF2 &app);
        static void fog(RFF2 &app);
        static void bloom(RFF2 &app);
        static void noiseReduction(RFF2 &app);
#ifdef USE_EXPERIMENTAL_3D
        static void fractal3D(RFF2 &app);
#endif
    };
} // namespace merutilm::rff2
