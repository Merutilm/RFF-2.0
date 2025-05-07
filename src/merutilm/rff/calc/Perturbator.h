//
// Created by Merutilm on 2025-05-08.
//

#pragma once
#include "../ui/RFFConstants.h"

struct Perturbator {
    static int logZoomToExp10(const double logZoom) {
        return -static_cast<int>(logZoom) - RFFConstants::Precision::PRECISION_ADDITION;
    }
};
