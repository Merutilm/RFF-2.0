//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include <functional>


namespace merutilm::rff2 {
    class RFFApplication;
    struct FnFractal {
        static void reference(RFFApplication &app);
        static void iterations(RFFApplication &app);
        static void sa(RFFApplication &app);
        static void mpa(RFFApplication &app);

        static void automaticIterations(RFFApplication &app);
        static void absoluteIterationMode(RFFApplication &app);
    };
} // namespace merutilm::rff2
