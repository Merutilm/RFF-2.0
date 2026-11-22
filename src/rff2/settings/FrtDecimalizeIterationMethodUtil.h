//
// Created by Merutilm on 2025-05-04.
//

#pragma once
#include <cmath>

#include "../constants/FractalConstants.hpp"

namespace merutilm::rff2::FrtDecimalizeIterationMethodUtil {


    inline double getExteriorDoubleValueIterationRatio(const double prevIterDistance, const double currIterDistance,
                                                       const double bailout) {
        // prevIterDistance = p
        // currIterDistance = c
        // bailout = b
        //
        // a = b - p (p < b)
        // b = c - b (c > b)
        // 0 dec 1 decimal value
        // a : b ratio
        // ratio = a / (a + b) = (b - p) / (c - p)

        if (prevIterDistance == currIterDistance) {
            return 0.0;
        }
        return bailout >= Constants::Fractal::BAILOUT_THRESHOLD_ITERATION_SMOOTHING_METHOD
                       ? 1 - log(log(currIterDistance) / std::numbers::ln2) / std::numbers::ln2
                       : log1p(bailout * (bailout - prevIterDistance) / (currIterDistance - prevIterDistance)) /
                                 log1p(bailout);
    }
} // namespace merutilm::rff2::FrtDecimalizeIterationMethodUtil
