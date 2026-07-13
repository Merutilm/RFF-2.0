//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include <functional>



namespace merutilm::rff2 {
    class RFFApplication;
    struct FnRender {
        static void setRenderProperties(RFFApplication &app);
        static void linearInterpolation(RFFApplication &app);
    };
}
