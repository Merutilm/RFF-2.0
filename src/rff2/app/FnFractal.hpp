//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include "RFF2.hpp"


namespace merutilm::rff2 {
    struct FnFractal {
        static void reference(RFF2 &app);
        static void iterations(RFF2 &app);
#ifdef RFF_USE_SA
        static void sa(RFF2 &app);
#endif
        static void mpa(RFF2 &app);

        static void automaticIterations(RFF2 &app);
        static void absoluteIterationMode(RFF2 &app);
    };
} // namespace merutilm::rff2
