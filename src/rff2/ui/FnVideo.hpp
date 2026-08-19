//
// Created by Merutilm on 2025-06-08.
//

#pragma once
#include "RFF2.hpp"

namespace merutilm::rff2 {
    struct FnVideo {
        static void dataSettings(RFF2 &app);
        static void animationSettings(RFF2 &app);
        static void exportSettings(RFF2 &app);
        static void generateVidKeyframes(RFF2 &app);
        static void exportZoomVideo(RFF2 &app);
    };
}
