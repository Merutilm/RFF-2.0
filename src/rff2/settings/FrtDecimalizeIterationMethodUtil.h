//
// Created by Merutilm on 2025-05-04.
//

#pragma once
#include <cmath>

namespace merutilm::rff2::FrtDecimalizeIterationMethodUtil {

    inline double applyDecimalize(const double bailout, const double ratio) {
        return log1p(bailout * ratio) / log1p(bailout);
    }

    inline double getExteriorDoubleValueIterationRatio(const double prevIterDistance, const double currIterDistance, const double bailout) {
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
        return applyDecimalize(bailout, (bailout - prevIterDistance) / (currIterDistance - prevIterDistance));
    }
} // namespace merutilm::rff2::FrtDecimalizeIterationMethodUtil
