//
// Created by Merutilm on 2025-05-16.
//

#pragma once
#include "RFFApplication.hpp"

namespace merutilm::rff2 {
    struct FnShader {
        static void palette(RFFApplication &app);
        static void stripe(RFFApplication &app);
        static void slope(RFFApplication &app);
        static void color(RFFApplication &app);
        static void fog(RFFApplication &app);
        static void bloom(RFFApplication &app);
    };
} // namespace merutilm::rff2
