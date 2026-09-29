//
// Created by Merutilm on 2025-05-08.
//

#pragma once
#include "../constants/Constants.hpp"

namespace merutilm::rff2 {
    struct Perturbator {

        virtual ~Perturbator() = default;


        static int64_t getExp10(const FrtReferenceSettings &refSettings, const double logZoom) {
            return refSettings.useFixedPrecision ? -refSettings.fixedPrecisionNeg : logZoomToExp10(logZoom);
        }

        static int64_t logZoomToExp10(const double logZoom){
            return -static_cast<int64_t>(logZoom) - Constants::Fractal::EXP10_ADDITION;
        }

    };


}