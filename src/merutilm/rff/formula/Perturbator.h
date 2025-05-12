//
// Created by Merutilm on 2025-05-08.
//

#pragma once
#include "../settings/DecimalizeIterationMethod.h"
#include "../ui/RFFConstants.h"

struct Perturbator {
    static int logZoomToExp10(const float logZoom) {
        return -static_cast<int>(logZoom) - RFFConstants::Precision::PRECISION_ADDITION;
    }
    static double getDoubleValueIteration(unsigned long long iteration, double prevIterDistance, double currIterDistance, const DecimalizeIterationMethod &decimalizeIterationMethod, float
                                          bailout);
};
