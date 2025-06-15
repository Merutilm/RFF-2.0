//
// Created by Merutilm on 2025-05-08.
//

#pragma once
#include "../settings/DecimalizeIterationMethod.h"
#include "../ui/Constants.h"

namespace merutilm::rff {
    struct Perturbator {

        virtual ~Perturbator() = default;

        static int logZoomToExp10(float logZoom);

        static double getDoubleValueIteration(unsigned long long iteration, double prevIterDistance,
                                              double currIterDistance,
                                              const DecimalizeIterationMethod &decimalizeIterationMethod, float
                                              bailout);
    };

    inline int Perturbator::logZoomToExp10(const float logZoom) {
        return -static_cast<int>(logZoom) - Constants::Render::EXP10_ADDITION;
    }
}