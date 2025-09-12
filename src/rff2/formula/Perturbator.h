//
// Created by Merutilm on 2025-05-08.
//

#pragma once
#include "../attr/FrtDecimalizeIterationMethod.h"
#include "../constants/Constants.hpp"

namespace merutilm::rff2 {
    struct Perturbator {

        virtual ~Perturbator() = default;

        static int logZoomToExp10(float logZoom);

        static double getDoubleValueIteration(uint64_t iteration, double prevIterDistance,
                                              double currIterDistance,
                                              const FrtDecimalizeIterationMethod &decimalizeIterationMethod, float
                                              bailout);
    };

    inline int Perturbator::logZoomToExp10(const float logZoom) {
        return -static_cast<int>(logZoom) - Constants::Fractal::EXP10_ADDITION;
    }

    inline double Perturbator::getDoubleValueIteration(const uint64_t iteration, const double prevIterDistance,
        const double currIterDistance, const FrtDecimalizeIterationMethod &decimalizeIterationMethod, const float bailout) {
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
            using enum FrtDecimalizeIterationMethod;
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
                ratio = log10(ratio + 1) / Constants::Num::LOG10_2;
                break;
            }
            case LOG_LOG : {
                constexpr double logBailout = Constants::Num::LOG10_2;
                ratio = log10(log10(ratio + 1) / logBailout + 1) / logBailout;
                break;
            }
            default : break;
        }

        return static_cast<double>(iteration) + ratio;
    }
}