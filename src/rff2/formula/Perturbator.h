//
// Created by Merutilm on 2025-05-08.
//

#pragma once
#include "../constants/Constants.hpp"
#include "../settings/FrtDecimalizeIterationMethod.h"

namespace merutilm::rff2 {
    struct Perturbator {

        virtual ~Perturbator() = default;

        static int logZoomToExp10(float logZoom);

    };

    inline int Perturbator::logZoomToExp10(const float logZoom) {
        return -static_cast<int>(logZoom) - Constants::Fractal::EXP10_ADDITION;
    }


}