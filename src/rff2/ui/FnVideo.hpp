//
// Created by Merutilm on 2025-06-08.
//

#pragma once
#include <functional>

#include "RFFApplication.hpp"

namespace merutilm::rff2 {
    struct FnVideo {
        static void dataSettings(RFFApplication &app);
        static void animationSettings(RFFApplication &app);
        static void exportSettings(RFFApplication &app);
        static void generateVidKeyframes(RFFApplication &app);
        static void exportZoomVideo(RFFApplication &app);
    };
}
