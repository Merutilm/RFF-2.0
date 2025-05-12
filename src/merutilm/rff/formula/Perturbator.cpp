//
// Created by Merutilm on 2025-05-08.
//

#include "Perturbator.h"

#include <cmath>

#include "../settings/DecimalizeIterationMethod.h"




double Perturbator::getDoubleValueIteration(const uint64_t iteration, const double prevIterDistance, const double currIterDistance, const DecimalizeIterationMethod &decimalizeIterationMethod, const float bailout) {
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
        return static_cast<double>(iteration);
    }
    double ratio = (bailout - prevIterDistance) / (currIterDistance - prevIterDistance);


    switch (decimalizeIterationMethod) {
        using enum DecimalizeIterationMethod;
        case NONE : {
            ratio = 0;
            break;
        }
        case LINEAR : {
            break;
        }
        case SQUARE_ROOT : {
            ratio = sqrt(ratio);
            break;
        }
        case LOG : {
            ratio = log(ratio + 1) / log(bailout);
            break;
        }
        case LOG_LOG : {
            const double logBailout = log(bailout);
            ratio = log(log(ratio + 1) / logBailout + 1) / logBailout;
            break;
        }
        default : break;
    }

    return static_cast<double>(iteration) + ratio;
}

