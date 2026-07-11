//
// Created by Merutilm on 2025-05-04.
//

#pragma once
#include "../calc/dex.h"
#include "../constants/Constants.hpp"

namespace merutilm::rff2 {
    enum class FrtDecimalizeIterationMethod {
        /**
         * Do Not Use Decimal Iterations.
         */
        NONE,
        /**
         * Use triangle inequality once.
         */
        LINEAR,
        /**
         * Calculates <b>Sqrt(Linear)</b>.
         */
        SQUARE_ROOT,
        /**
         * Calculates <b>Log(Linear + 1)</b>.
         */
        LOG,
        /**
         * Calculates <b>Log(Log(Linear + 1) + 1)</b>.
         */
        LOG_LOG
    };


    namespace FrtDecimalizeIterationMethodUtil {


        inline double applyDecimalize(const FrtDecimalizeIterationMethod decimalizeIterationMethod, double ratio) {
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
            return ratio;
        }

        inline double getExteriorDoubleValueIterationRatio(const double prevIterDistance, const double currIterDistance,
                                                           const FrtDecimalizeIterationMethod decimalizeIterationMethod,
                                                           const float bailout) {
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
            return applyDecimalize(decimalizeIterationMethod, (bailout - prevIterDistance) / (currIterDistance - prevIterDistance));
        }
    }
}
