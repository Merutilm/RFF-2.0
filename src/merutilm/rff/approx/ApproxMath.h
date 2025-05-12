//
// Created by Merutilm on 2025-05-09.
//

#pragma once
#include <cmath>

struct ApproxMath {

    ApproxMath() = delete;
    static double hypotApproximate(double x, double y) {
        x = fabs(x);
        y = fabs(y);
        const double min = std::min(x, y);
        const double max = std::max(x, y);

        if (min == 0) {
            return max;
        }
        if (max == 0) {
            return 0;
        }

        return max + 0.428 * min / max * min;
    };
};
