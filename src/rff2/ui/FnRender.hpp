//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include <functional>



namespace merutilm::rff2 {
    class RFF2;
    struct FnRender {
        static void setResolutionProperties(RFF2 &app);
        static void setRenderProperties(RFF2 &app);
        static void linearInterpolation(RFF2 &app);
    };
}
