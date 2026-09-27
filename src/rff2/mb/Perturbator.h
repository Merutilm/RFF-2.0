//
// Created by Merutilm on 2025-05-08.
//

#pragma once
#include "../constants/Constants.hpp"

namespace merutilm::rff2 {
    struct Perturbator {

        virtual ~Perturbator() = default;

        static int64_t logZoomToExp10(double logZoom);

    };

    inline int64_t Perturbator::logZoomToExp10(const double logZoom) {
        return -static_cast<int64_t>(logZoom) - Constants::Fractal::EXP10_ADDITION;
    }


}